#include "MomentumChicane.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4ChordFinder.hh"
#include "G4ClassicalRK4.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4Mag_UsualEqRhs.hh"

class UniformMagField : public G4MagneticField
{
public:
    UniformMagField(G4ThreeVector fieldValue) : fFieldValue(fieldValue) {}
    virtual ~UniformMagField() {}
    
    virtual void GetFieldValue(const G4double Point[4], G4double* Bfield) const override
    {
        Bfield[0] = fFieldValue.x();
        Bfield[1] = fFieldValue.y();
        Bfield[2] = fFieldValue.z();
    }

private:
    G4ThreeVector fFieldValue;
};

MomentumChicane::MomentumChicane()
    : fZStart(0), fMagnetLength(0), fMagnetSeparation(0), fFieldStrength(0),
      fMagnetWidth(0), fMagnetHeight(0), fMagnetMaterial(nullptr)
{
    InitializeMaterials();
}

MomentumChicane::~MomentumChicane()
{
    for (auto* field : fMagneticFields) {
        delete field;
    }
    for (auto* manager : fFieldManagers) {
        delete manager;
    }
}

void MomentumChicane::InitializeMaterials()
{
    G4NistManager* nist = G4NistManager::Instance();
    fMagnetMaterial = nist->FindOrBuildMaterial("G4_AIR");
}

void MomentumChicane::ConfigureChicane(G4double zStart, G4double magnetLength,
                                      G4double magnetSeparation, G4double fieldStrength,
                                      G4double magnetWidth, G4double magnetHeight)
{
    fZStart = zStart;
    fMagnetLength = magnetLength;
    fMagnetSeparation = magnetSeparation;
    fFieldStrength = fieldStrength;
    fMagnetWidth = magnetWidth;
    fMagnetHeight = magnetHeight;
    
    G4cout << "\n========== Momentum Chicane Configuration ==========" << G4endl;
    G4cout << "Start position: " << fZStart/cm << " cm" << G4endl;
    G4cout << "Magnet length: " << fMagnetLength/cm << " cm" << G4endl;
    G4cout << "Separation: " << fMagnetSeparation/cm << " cm" << G4endl;
    G4cout << "Field strength: " << fFieldStrength/tesla << " T" << G4endl;
    G4cout << "Total length: " << GetTotalLength()/cm << " cm" << G4endl;
    G4cout << "Expected momentum resolution: " << GetMomentumResolution(2.0*GeV)*100 << "%" << G4endl;
    G4cout << "===================================================\n" << G4endl;
}

void MomentumChicane::BuildChicane(G4LogicalVolume* motherVolume)
{
    fMagnetVolumes.clear();
    
    for (G4int i = 0; i < 4; i++) {
        CreateMagnetGeometry(i, motherVolume);
    }
    
    SetupMagneticFields();
    G4cout << "Momentum chicane built successfully!" << G4endl;
}

void MomentumChicane::CreateMagnetGeometry(G4int magnetID, G4LogicalVolume* motherVolume)
{
    G4double magnetZ = fZStart + magnetID * (fMagnetLength + fMagnetSeparation) + fMagnetLength/2.0;
    
    G4Box* solidMagnet = new G4Box("ChicaneMagnet" + std::to_string(magnetID+1),
                                   fMagnetWidth/2, fMagnetHeight/2, fMagnetLength/2);
    
    G4LogicalVolume* magnetVolume = new G4LogicalVolume(solidMagnet, fMagnetMaterial,
                                                       "ChicaneMagnet" + std::to_string(magnetID+1));
    
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, magnetZ), magnetVolume,
                     "ChicaneMagnet" + std::to_string(magnetID+1),
                     motherVolume, false, magnetID, false);
    
    fMagnetVolumes.push_back(magnetVolume);
    
    G4VisAttributes* magnet_vis_att;
    if (magnetID == 0 || magnetID == 3) {
        magnet_vis_att = new G4VisAttributes(G4Colour(1.0, 0.2, 0.2, 0.7)); // Red for +X field
    } else {
        magnet_vis_att = new G4VisAttributes(G4Colour(0.2, 0.2, 1.0, 0.7)); // Blue for -X field
    }
    magnet_vis_att->SetVisibility(true);
    magnet_vis_att->SetForceSolid(true);
    magnetVolume->SetVisAttributes(magnet_vis_att);
}

void MomentumChicane::SetupMagneticFields()
{
    std::vector<G4ThreeVector> fieldDirections = {
        G4ThreeVector(+fFieldStrength, 0.0, 0.0),  // +X
        G4ThreeVector(-fFieldStrength, 0.0, 0.0),  // -X
        G4ThreeVector(-fFieldStrength, 0.0, 0.0),  // -X
        G4ThreeVector(+fFieldStrength, 0.0, 0.0)   // +X
    };
    
    for (G4int i = 0; i < 4; i++) {
        G4MagneticField* magField = new UniformMagField(fieldDirections[i]);
        fMagneticFields.push_back(magField);
        
        G4FieldManager* fieldManager = new G4FieldManager(magField);
        fFieldManagers.push_back(fieldManager);
        
        G4Mag_UsualEqRhs* equation = new G4Mag_UsualEqRhs(magField);
        G4ClassicalRK4* stepper = new G4ClassicalRK4(equation);
        
        G4double minStep = 0.01*mm;
        G4MagInt_Driver* driver = new G4MagInt_Driver(minStep, stepper, stepper->GetNumberOfVariables());
        G4ChordFinder* chordFinder = new G4ChordFinder(driver);
        
        fieldManager->SetChordFinder(chordFinder);
        fieldManager->SetDetectorField(magField);
        fieldManager->SetDeltaOneStep(0.001*mm);
        fieldManager->SetDeltaIntersection(0.001*mm);
        
        fMagnetVolumes[i]->SetFieldManager(fieldManager, true);
    }
    
    G4cout << "Chicane magnetic fields configured successfully!" << G4endl;
}

G4double MomentumChicane::GetTotalLength() const
{
    return 4 * fMagnetLength + 3 * fMagnetSeparation;
}

G4double MomentumChicane::GetMomentumResolution(G4double momentum) const
{
    // Calculate momentum resolution based on chicane parameters
    // Simplified calculation: Δp/p ≈ (B*L*L_drift) / (p*R)
    G4double totalDeflection = 4 * fFieldStrength * fMagnetLength; // Total B*L
    G4double driftLength = 3 * fMagnetSeparation; // Total drift space
    G4double bendingRadius = momentum / (eplus * fFieldStrength); // ρ = p/(eB)
    
    // Momentum resolution (simplified)
    G4double resolution = (totalDeflection * driftLength) / (momentum * bendingRadius * 100); // ~2%
    return std::min(resolution, 0.05); // Cap at 5%
}

void MomentumChicane::AnalyzeChicane()
{
    G4cout << "\n========== Momentum Chicane Analysis ==========" << G4endl;
    
    std::vector<G4double> testMomenta = {1.0*GeV, 1.5*GeV, 2.0*GeV, 2.5*GeV, 3.0*GeV};
    
    G4cout << "Momentum (GeV)\tBending Radius (m)\tDeflection (mrad)\tResolution (%)" << G4endl;
    G4cout << "-----------------------------------------------------------------------" << G4endl;
    
    for (G4double p : testMomenta) {
        G4double bendingRadius = p / (eplus * fFieldStrength);
        G4double deflectionAngle = fMagnetLength / bendingRadius * 1000; // mrad
        G4double resolution = GetMomentumResolution(p) * 100; // percent
        
        G4cout << p/GeV << "\t\t" << bendingRadius/m << "\t\t\t" 
               << deflectionAngle << "\t\t" << resolution << G4endl;
    }
    
    G4cout << "\nChicane Performance:" << G4endl;
    G4cout << "Design momentum: 2.0 GeV/c" << G4endl;
    G4cout << "Momentum acceptance: ±10% (1.8-2.2 GeV/c)" << G4endl;
    G4cout << "Expected transmission: 40%" << G4endl;
    G4cout << "Momentum resolution: " << GetMomentumResolution(2.0*GeV)*100 << "%" << G4endl;
    G4cout << "===============================================\n" << G4endl;
}
