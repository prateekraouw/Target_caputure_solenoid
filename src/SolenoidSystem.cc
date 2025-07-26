#include "SolenoidSystem.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Box.hh"
#include <cmath>

SolenoidSystem::SolenoidSystem()
    : fAirMaterial(nullptr), fCopperMaterial(nullptr), fInsulatorMaterial(nullptr)
{
    InitializeMaterials();
}

SolenoidFringeField::SolenoidFringeField(G4double Bz0, G4double radius, G4double length, G4double zCenter)
    : fBz0(Bz0), fRadius(radius), fLength(length), fZCenter(zCenter)
{
}

SolenoidSystem::~SolenoidSystem()
{
    CleanupFieldObjects();
}

void SolenoidSystem::CleanupFieldObjects()
{
    // Clean up in reverse order of creation to avoid dependencies
    for (auto* chordFinder : fChordFinders) {
        delete chordFinder;
    }
    fChordFinders.clear();

    for (auto* driver : fDrivers) {
        delete driver;
    }
    fDrivers.clear();

    for (auto* stepper : fSteppers) {
        delete stepper;
    }
    fSteppers.clear();

    for (auto* equation : fEquations) {
        delete equation;
    }
    fEquations.clear();

    for (auto* fieldManager : fFieldManagers) {
        delete fieldManager;
    }
    fFieldManagers.clear();

    for (auto* magField : fMagneticFields) {
        delete magField;
    }
    fMagneticFields.clear();
}

void SolenoidSystem::InitializeMaterials()
{
    G4NistManager* nist = G4NistManager::Instance();
    fAirMaterial = nist->FindOrBuildMaterial("G4_AIR");
    fCopperMaterial = nist->FindOrBuildMaterial("G4_Cu");

    // Create simple insulator material (plastic-like)
    G4Material* carbon = nist->FindOrBuildMaterial("G4_C");
    G4Material* hydrogen = nist->FindOrBuildMaterial("G4_AIR");
    fInsulatorMaterial = new G4Material("Insulator", 1.2*g/cm3, 2);
    fInsulatorMaterial->AddMaterial(carbon, 85*perCent);
    fInsulatorMaterial->AddMaterial(hydrogen, 15*perCent);
}

void SolenoidFringeField::GetFieldValue(const G4double point[4], G4double* field) const
{
    G4double x = point[0];
    G4double y = point[1];
    G4double z = point[2];

    G4double rho = std::sqrt(x*x + y*y);
    G4double zRel = z - fZCenter;

    // Initialize field components
    field[0] = 0.0; // Bx
    field[1] = 0.0; // By
    field[2] = 0.0; // Bz
    field[3] = 0.0; // Not used
    field[4] = 0.0; // Not used
    field[5] = 0.0; // Not used

    // Calculate field with fringe effects
    CalculateFringeField(rho, zRel, field);

    // Apply cylindrical symmetry for radial components
     if (rho > 0) {
        G4double Br = field[0];
        field[0] = Br * x / rho *tesla;  // Br * cos(phi)
        field[1] = Br * y / rho *tesla;  // Br * sin(phi)
        }
}

// Calculate magnetic field from physical solenoid parameters
G4double SolenoidSystem::CalculateMagneticField(G4double Rc, G4double DZ, G4int NR, G4int NZ, G4double current)
{
    // Total number of turns
    G4int totalTurns = NR * NZ;

    // Magnetic permeability of free space
    const G4double mu0 = 4.0 * M_PI * 1e-7 * tesla * m / ampere;

    // For a finite solenoid at the center:
    // B = (μ₀ * n * I) * [L / sqrt(L² + 4R²)]
    // where n = N/L (turns per unit length)
    G4double turnsPerLength = totalTurns / DZ;
    G4double kiloFactor = 1000;

    // Calculate field strength
    G4double magneticField = mu0 * turnsPerLength * current * kiloFactor;

    G4cout << "  Magnetic field calculation:" << G4endl;
    G4cout << "    Rc = " << Rc/cm << " cm, DZ = " << DZ/cm << " cm" << G4endl;
    G4cout << "    Total turns (NR×NZ) = " << NR << "×" << NZ << " = " << totalTurns << G4endl;
    G4cout << "    Turns per length = " << turnsPerLength*m << " turns/m" << G4endl;
    G4cout << "    Current = " << current/ampere << " A" << G4endl;
    G4cout << "    Length factor = " << kiloFactor << G4endl;
    G4cout << "    Calculated B-field = " << magneticField/tesla << " T" << G4endl;

    return magneticField;
}

void SolenoidSystem::CreateRampUpSolenoids(G4double Rc, G4double Zc, G4double DR, G4double DZ,
                                          G4int NR, G4int NZ, G4double current,
                                          G4LogicalVolume* motherVolume)
{
    G4cout << "\n========== Creating Single Physical Solenoid ==========" << G4endl;
    CleanupFieldObjects();
    fMagFieldVolumes.clear();
    fFieldStrengths.clear();
    fSolenoidParameters.clear();

    // Create solenoid parameters from direct input
    SolenoidParameters params;
    params.Rc = Rc;
    params.Zc = Zc;
    params.DR = DR;
    params.DZ = DZ;
    params.NR = NR;
    params.NZ = NZ;
    params.current = current;

    // Calculate magnetic field from the given parameters
    G4double calculatedField = CalculateMagneticField(Rc, DZ, NR, NZ, current);

    G4cout << "\n--- Creating Physical Solenoid ---" << G4endl;
    G4cout << "Input Parameters:" << G4endl;
    G4cout << "  Rc (coil radius): " << Rc/m << " m" << G4endl;
    G4cout << "  Zc (center position): " << Zc/m << " m" << G4endl;
    G4cout << "  DR (radial extent): " << DR/m << " m" << G4endl;
    G4cout << "  DZ (axial extent): " << DZ/m << " m" << G4endl;
    G4cout << "  NR × NZ (turns): " << NR << " × " << NZ << " = " << (NR * NZ) << G4endl;
    G4cout << "  Current: " << current/ampere << " A" << G4endl;
    G4cout << "  Calculated B-field: " << calculatedField/tesla << " T" << G4endl;

    // Create the complete solenoid (coil + field volume)
    G4LogicalVolume* fieldVolume = CreateAirSolenoid(params, motherVolume);

    fMagFieldVolumes.push_back(fieldVolume);
    fFieldStrengths.push_back(calculatedField);
    fSolenoidParameters.push_back(params);

    CreateFieldManagers();

    G4cout << "\n========== Physical Solenoid Created ==========" << G4endl;
    G4cout << "Solenoid specifications:" << G4endl;
    G4cout << "  Coil geometry: Rc=" << Rc/m << "m, DR=" << DR/m << "m, DZ=" << DZ/m << "m" << G4endl;
    G4cout << "  Electrical: " << (NR*NZ) << " turns, " << current/ampere << "A" << G4endl;
    G4cout << "  Magnetic field: " << calculatedField/tesla << "T" << G4endl;
}


G4LogicalVolume* SolenoidSystem::CreateAirSolenoid(const SolenoidParameters& params,
                                                  G4LogicalVolume* motherVolume)
{
    // Generate unique ID based on current number of solenoids
    G4int solenoidID = fMagFieldVolumes.size() + 1;
    G4String baseName = "Solenoid_" + std::to_string(solenoidID);

    // 1. Create the field volume (air-filled region where field exists)
    G4double fieldRadius = params.Rc - params.DR/2.0;  // Field extends slightly inside coil
    G4String fieldName = baseName + "_FieldVolume";

    G4VSolid* fieldSolid = new G4Tubs(fieldName + "_solid",
                                     0,                    // Inner radius (hollow for beam)
                                     fieldRadius,          // Outer radius
                                     params.DZ/2.0,       // Half length
                                     0, 2*M_PI);           // Full circle

    G4LogicalVolume* fieldLogical = new G4LogicalVolume(fieldSolid, fAirMaterial, fieldName);

    // Place field volume
    new G4PVPlacement(0, G4ThreeVector(0, 0, params.Zc), fieldLogical,
                     fieldName, motherVolume, false, solenoidID, true);

    // 2. Create the actual coil structure
    CreateCoilStructure(solenoidID, params, motherVolume);

    // 3. Set visualization for field volume
    SetAirSolenoidVisualization(fieldLogical, solenoidID, params.Rc);

    G4cout << "  Created complete solenoid with field volume: " << fieldLogical->GetName() << G4endl;

    return fieldLogical;
}

void SolenoidSystem::CreateCoilStructure(G4int solenoidID, const SolenoidParameters& params,
                                        G4LogicalVolume* motherVolume)
{
    G4String baseName = "Solenoid_" + std::to_string(solenoidID);

    // Calculate coil dimensions
    G4double innerCoilRadius = params.Rc - params.DR/2.0;
    G4double outerCoilRadius = params.Rc + params.DR/2.0;

    // Create coil winding volume (copper + insulator mixture)
    G4String coilName = baseName + "_Coil";
    G4VSolid* coilSolid = new G4Tubs(coilName + "_solid",
                                    innerCoilRadius,      // Inner radius
                                    outerCoilRadius,      // Outer radius
                                    params.DZ/2.0,       // Half length
                                    0, 2*M_PI);           // Full circle

    // Create composite coil material (approximation of copper windings + insulation)
    G4Material* coilMaterial = CreateCoilMaterial(params.NR, params.NZ);
    G4LogicalVolume* coilLogical = new G4LogicalVolume(coilSolid, coilMaterial, coilName);

    // Place coil
    new G4PVPlacement(0, G4ThreeVector(0, 0, params.Zc), coilLogical,
                     coilName, motherVolume, false, solenoidID + 1000, true);

    // Set coil visualization
    G4VisAttributes* coilVis = new G4VisAttributes(G4Color(0.8, 0.4, 0.1, 0.8));  // Copper color
    coilVis->SetVisibility(true);
    coilVis->SetForceSolid(true);
    coilLogical->SetVisAttributes(coilVis);

    G4cout << "  Created coil structure: " << coilName << G4endl;
    G4cout << "    Inner/Outer radius: " << innerCoilRadius/cm << "/" << outerCoilRadius/cm << " cm" << G4endl;
    G4cout << "    Material: " << coilMaterial->GetName() << G4endl;
}

G4Material* SolenoidSystem::CreateCoilMaterial(G4int NR, G4int NZ)
{
    // Calculate packing factors
    G4double copperFraction = 0.90;  // Typical packing factor for copper in coils
    G4double insulatorFraction = 0.09;
    G4double airFraction = 0.01;

    // Create composite material name
    G4String materialName = "CoilMaterial_NR" + std::to_string(NR) + "_NZ" + std::to_string(NZ);

    // Check if material already exists
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* existingMaterial = nist->FindOrBuildMaterial(materialName);
    if (existingMaterial) {
        return existingMaterial;
    }

    // Create new composite material
    G4double density = copperFraction * fCopperMaterial->GetDensity() +
                      insulatorFraction * fInsulatorMaterial->GetDensity() +
                      airFraction * fAirMaterial->GetDensity();

    G4Material* coilMaterial = new G4Material(materialName, density, 3);
    coilMaterial->AddMaterial(fCopperMaterial, copperFraction);
    coilMaterial->AddMaterial(fInsulatorMaterial, insulatorFraction);
    coilMaterial->AddMaterial(fAirMaterial, airFraction);

    return coilMaterial;
}

void SolenoidSystem::SetAirSolenoidVisualization(G4LogicalVolume* solenoidLogical,
                                                G4int solenoidID, G4double radius)
{
    // Visualization based on field strength and position
    G4double maxRadius = 0.8*m;  // 80 cm
    G4double minRadius = 0.1*m;  // 10 cm
    G4double fraction = (radius - minRadius) / (maxRadius - minRadius);

    // Blue → Red gradient for field visualization
    G4double red = 1.0;   // More red for smaller radius (stronger field)
    G4double green = 1.0;         // Slight green component
    G4double blue = 1.0;          // More blue for larger radius
    G4double alpha = 0;                           // Semi-transparent for field visualization

    G4VisAttributes* fieldVis = new G4VisAttributes(G4Color(red,green,blue,alpha));
    fieldVis->SetVisibility(true);
    fieldVis->SetForceSolid(false);  // Wireframe for field volume
    fieldVis->SetForceWireframe(false);
    solenoidLogical->SetVisAttributes(fieldVis);

    G4cout << "  Field visualization: radius " << radius/cm
           << " cm → RGB(" << red << ", " << green << ", " << blue << ")" << G4endl;
}

G4double SolenoidFringeField::FringeFieldFactor(G4double z) const
{
    G4double zEnd1 = -fLength/2.0;
    G4double zEnd2 = +fLength/2.0;
    G4double fringeWidth = fRadius; // Fringe field width 100% of radius

    if (z < zEnd1 - fringeWidth || z > zEnd2 + fringeWidth) {
        return 0.0; // No field outside fringe region
    }
    else if(z >=zEnd1  && z <= zEnd2 ) {
        return 1.0; // Full field in central region
    }
    else {
        // Smooth transition in fringe regions using hyperbolic tangent
        if (z < zEnd1) {
            // Left fringe region
            G4double s = (z - zEnd1 + fringeWidth) / (2.0 * fringeWidth);
            return 0.5 * (1.0 + std::tanh(2 * (s - 0.5)));
        } else {
            // Right fringe region
            G4double s = (zEnd2 + fringeWidth - z) / (2.0 * fringeWidth);
            return 0.5 * (1.0 + std::tanh(2 * (s - 0.5)));
        }
    }
}

void SolenoidFringeField::CalculateFringeField(G4double rho, G4double z, G4double* field) const
{

    //Aperture definition
    G4double systemLength = 18.79*m;
    G4double systemCenter = 8.095*m;
    G4double zPoint = z - systemCenter;
    G4double endradius = 0.30*m;
    G4double startRadius = 0.075*m;
    G4double halfLength = systemLength / 2;
    G4double fraction = (zPoint + halfLength) / systemLength;

    fraction = std::max(0.0, std::min(1.0, fraction));

    G4double Rz = startRadius + (endradius - startRadius) * fraction;

    // Get the fringe factor for axial position
    G4double fringeFactor = FringeFieldFactor(z);

    if (fringeFactor <= 0.0) {
        return; // No field
    }

    /*if(rho > Rz){
        field[0]=field[1]=field[2]= 0.0;
        return;
        }*/

    field[2] = fBz0*fringeFactor;

    // Radial field component from fringe effects
    // Br = -(rho/2) * dBz/dz
    if (rho > 0) {
        G4double deltaZ = 0.001 * Rz; // Small step for numerical derivative
        G4double fringeFactorPlus = FringeFieldFactor(z + deltaZ);
        G4double fringeFactorMinus = FringeFieldFactor(z - deltaZ);

        G4double dBz_dz = fBz0 * (fringeFactorPlus - fringeFactorMinus) / (2.0 * deltaZ);
        field[0] = -0.5 * rho * dBz_dz; // Radial component magnitude

        }
}

void SolenoidSystem::CreateFieldManagers()
{

    // Get the most recently created solenoid (last in vectors)
    size_t index = fMagFieldVolumes.size() - 1;
    G4LogicalVolume* solenoidVolume = fMagFieldVolumes[index];
    G4double fieldStrength = fFieldStrengths[index];
    const SolenoidParameters& params = fSolenoidParameters[index];

    G4cout << "Volume: " << solenoidVolume->GetName() << G4endl;
    G4cout << "Field: " << fieldStrength/tesla << " T" << G4endl;
    G4cout << "Current: " << params.current/ampere << " A" << G4endl;
    G4cout << "Turns: " << params.NR << "×" << params.NZ << " = " << (params.NR*params.NZ) << G4endl;

    // Create solenoid field with fringe effects instead of uniform field
    G4double solenoidRadius = params.Rc - params.DR/2.0; // Field radius
    SolenoidFringeField* magField = new SolenoidFringeField(
        fieldStrength,      // Central field strength
        solenoidRadius,     // Solenoid radius
        params.DZ,          // Solenoid length
        params.Zc           // Center position
    );
    fMagneticFields.push_back(magField);

    G4cout << "Created fringe field with parameters:" << G4endl;
    G4cout << "  Central field: " << fieldStrength/tesla << " T" << G4endl;
    G4cout << "  Field radius: " << solenoidRadius/cm << " cm" << G4endl;
    G4cout << "  Solenoid length: " << params.DZ/cm << " cm" << G4endl;
    G4cout << "  Center position: " << params.Zc/cm << " cm" << G4endl;

    // Create and track field manager
    G4FieldManager* fieldManager = new G4FieldManager(magField);
    fFieldManagers.push_back(fieldManager);

    // Create and track equation of motion
    G4Mag_UsualEqRhs* equation = new G4Mag_UsualEqRhs(magField);
    fEquations.push_back(equation);

    // Create and track stepper
    G4ClassicalRK4* stepper = new G4ClassicalRK4(equation);
    fSteppers.push_back(stepper);

    // Optimized precision for fringe field tracking
    G4double minStep = 0.005*mm; // Smaller step for fringe field accuracy
    G4MagInt_Driver* driver = new G4MagInt_Driver(minStep, stepper, stepper->GetNumberOfVariables());
    fDrivers.push_back(driver);

    // Create and track chord finder
    G4ChordFinder* chordFinder = new G4ChordFinder(driver);
    fChordFinders.push_back(chordFinder);

    // Configure field manager with tighter tolerances for fringe fields
    fieldManager->SetChordFinder(chordFinder);
    fieldManager->SetDetectorField(magField);
    fieldManager->SetDeltaOneStep(0.001*mm);
    fieldManager->SetDeltaIntersection(0.001*mm);

    // Apply field manager to field volume
    solenoidVolume->SetFieldManager(fieldManager, true);

    // Verify field application
    G4FieldManager* appliedManager = solenoidVolume->GetFieldManager();
    if (appliedManager) {
        G4cout << "Fringe field " << fieldStrength/tesla << " T applied to volume" << G4endl;
    } else {
        G4cout << "WARNING: Field manager not applied!" << G4endl;
    }
}

std::vector<G4LogicalVolume*> SolenoidSystem::GetMagFieldVolumes() const
{
    return fMagFieldVolumes;
}

void SolenoidSystem::PrintSystemInfo() const
{
    G4cout << "\n========== Complete Physical Solenoid System Info ==========" << G4endl;
    G4cout << "Total solenoids: " << fMagFieldVolumes.size() << G4endl;
    G4cout << "Total field managers: " << fFieldManagers.size() << G4endl;

    G4cout << "\nDetailed solenoid specifications:" << G4endl;
    for (size_t i = 0; i < fMagFieldVolumes.size(); i++) {
        const SolenoidParameters& params = fSolenoidParameters[i];
        G4LogicalVolume* volume = fMagFieldVolumes[i];
        G4FieldManager* manager = volume->GetFieldManager();

        G4cout << "\n  [" << (i+1) << "] " << volume->GetName() << G4endl;
        G4cout << "    Physical Parameters:" << G4endl;
        G4cout << "      Rc=" << params.Rc/m << "m, DR=" << params.DR/m << "m, DZ=" << params.DZ/m << "m" << G4endl;
        G4cout << "      NR×NZ=" << params.NR << "×" << params.NZ << "=" << (params.NR*params.NZ) << " turns" << G4endl;
        G4cout << "      Current=" << params.current/ampere << "A" << G4endl;
        G4cout << "    Performance:" << G4endl;
        G4cout << "      Field=" << fFieldStrengths[i]/tesla << "T" << G4endl;
        G4cout << "      Field Manager: " << (manager ? "✅" : "❌") << G4endl;

        // Calculate power dissipation (approximate)
        G4double resistance = 0.1 * ohm;  // Approximate coil resistance
        G4double power = params.current * params.current * resistance;
        G4cout << "      Power≈" << power/watt << "W" << G4endl;
    }

    G4cout << "\nSystem implements complete physics-based solenoid design:" << G4endl;
    G4cout << "• Realistic coil geometry with copper windings" << G4endl;
    G4cout << "• Accurate magnetic field calculations" << G4endl;
    G4cout << "• Proper material composition" << G4endl;
    G4cout << "• Optimized field managers for particle transport" << G4endl;
    G4cout << "================================================================\n" << G4endl;
}
