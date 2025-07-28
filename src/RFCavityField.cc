#include "RFCavityField.hh"
#include "G4NistManager.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

namespace {
    constexpr G4double kRadius         = 6*cm;
    constexpr G4double kOuterRadius    = 70*cm;
    constexpr G4double kLength         = 12.5*cm;
    constexpr G4double kAmplitudeConst = 20*megavolt/m;
    constexpr G4double kWinThickness   = 60*um;
    constexpr G4double kWinRadius      = 6*cm;
    constexpr G4double kMinStep        = 0.01*mm;
}

RFCavityField::RFCavityField(G4LogicalVolume* mother,
                             G4double        z0,
                             G4double        frequency,
                             G4double        phase)
 : G4ElectroMagneticField(),
   fMother(mother), fZ0(z0), fFrequency(frequency), fPhase(phase),
   fShellLV(nullptr), fInnerLV(nullptr), fWin1LV(nullptr), fWin2LV(nullptr),
   fFieldMgr(nullptr)
{
    BuildGeometry();
    BuildField();
}

RFCavityField::~RFCavityField()
{
    delete fFieldMgr;
    for (int i = (int)fChordFinders.size()-1; i >= 0; --i) delete fChordFinders[i];
    for (int i = (int)fDrivers.size()-1;      i >= 0; --i) delete fDrivers[i];
    for (int i = (int)fSteppers.size()-1;     i >= 0; --i) delete fSteppers[i];
    for (int i = (int)fEquations.size()-1;    i >= 0; --i) delete fEquations[i];
}

void RFCavityField::BuildGeometry()
{
    G4double halfLen = 0.5*kLength;
    G4double halfWin = 0.5*kWinThickness;

    // Copper shell
    auto shellSolid = new G4Tubs("RFShell", kRadius, kOuterRadius, halfLen, 0, 360*deg);
    fShellLV = new G4LogicalVolume(shellSolid,
                                   G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
                                   "RFShell");
    new G4PVPlacement(nullptr, G4ThreeVector(0,0,fZ0), fShellLV,
                      "RFShell", fMother, false, 0, true);

    // Vacuum bore
    auto innerSolid = new G4Tubs("RFInner", 0, kRadius, halfLen, 0, 360*deg);
    fInnerLV = new G4LogicalVolume(innerSolid,
                                   G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),
                                   "RFInner");
    new G4PVPlacement(nullptr, G4ThreeVector(0,0,0), fInnerLV,
                      "RFInner", fShellLV, false, 0, true);

    // Beryllium windows
    auto winSolid = new G4Tubs("RFWindow", 0, kWinRadius, halfWin, 0, 360*deg);
    fWin1LV = new G4LogicalVolume(winSolid,
                                   G4NistManager::Instance()->FindOrBuildMaterial("G4_Be"),
                                   "RFWindow1");
    fWin2LV = new G4LogicalVolume(winSolid,
                                   G4NistManager::Instance()->FindOrBuildMaterial("G4_Be"),
                                   "RFWindow2");
    new G4PVPlacement(nullptr, G4ThreeVector(0,0,fZ0 - (halfLen + halfWin)),
                      fWin1LV, "RFWindow1", fMother, false, 0, true);
    new G4PVPlacement(nullptr, G4ThreeVector(0,0,fZ0 + (halfLen + halfWin)),
                      fWin2LV, "RFWindow2", fMother, false, 0, true);

    // Visualization
    auto visShell = new G4VisAttributes(G4Colour(1,0.5,0,0.7));
    visShell->SetVisibility(true); visShell->SetForceSolid(true);
    fShellLV->SetVisAttributes(visShell);

    auto visInner = new G4VisAttributes(G4Colour(0.5,0.5,0.5,1));
    visInner->SetVisibility(true); visInner->SetForceWireframe(true);
    fInnerLV->SetVisAttributes(visInner);

    auto visWin = new G4VisAttributes(G4Colour(0,0,1,0.3));
    visWin->SetVisibility(true); visWin->SetForceSolid(true);
    fWin1LV->SetVisAttributes(visWin);
    fWin2LV->SetVisAttributes(visWin);
}

void RFCavityField::BuildField()
{
    // Equation of motion (E and B)
    auto eq = new G4EqMagElectricField(this);
    fEquations.push_back(eq);

    // RK4 stepper
    auto stepper = new G4ClassicalRK4(eq, /*nvar=*/8);
    fSteppers.push_back(stepper);

    // Integrator driver
    G4MagInt_Driver* driver = new G4MagInt_Driver(0.01*mm, stepper, stepper->GetNumberOfVariables());
    fDrivers.push_back(driver);

    // Chord finder
    auto chord = new G4ChordFinder(driver);
    fChordFinders.push_back(chord);

    // Field manager
    fFieldMgr = new G4FieldManager(this);
    fFieldMgr->SetChordFinder(chord);

    // Attach to vacuum bore only
    fInnerLV->SetFieldManager(fFieldMgr, true);
}

void RFCavityField::GetFieldValue(const G4double Point[4], G4double* field) const
{
    G4double time     = Point[3];
    G4double omega    = fFrequency * twopi;
    G4double strength = kAmplitudeConst * std::sin(omega*time + fPhase);

    field[0] = 0;
    field[1] = 0;
    field[2] = strength;
    field[3] = field[4] = field[5] = 0;
}
