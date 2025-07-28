#ifndef RFCAVITYFIELD_HH
#define RFCAVITYFIELD_HH

#include "G4ElectroMagneticField.hh"
#include "G4LogicalVolume.hh"
#include "G4FieldManager.hh"
#include "G4EqMagElectricField.hh"
#include "G4ClassicalRK4.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4ChordFinder.hh"
#include <vector>

class RFCavityField : public G4ElectroMagneticField {
public:
    /**
     * Construct an RF cavity + field inside.
     * @param mother    Parent logical volume (e.g. world)
     * @param z0        Cavity center in z
     * @param frequency RF frequency
     * @param phase     RF phase
     */
    RFCavityField(G4LogicalVolume* mother,
                  G4double z0,
                  G4double frequency,
                  G4double phase = 0.0);
    ~RFCavityField() override;

    // G4ElectroMagneticField interface
    void GetFieldValue(const G4double Point[4], G4double* field) const override;
    G4bool DoesFieldChangeEnergy() const override { return true; }

private:
    void InitializeMaterials();
    void BuildGeometry();
    void BuildField();

    G4LogicalVolume* fMother;
    G4LogicalVolume* fShellLV;
    G4LogicalVolume* fInnerLV;
    G4LogicalVolume* fWin1LV;
    G4LogicalVolume* fWin2LV;

    // RF parameters
    G4double fZ0;
    G4double fFrequency;
    G4double fPhase;

    // Materials
    G4Material* fCopperMat;
    G4Material* fVacuumMat;
    G4Material* fBeMat;

    // Field and integrator objects
    G4FieldManager* fFieldMgr;
    std::vector<G4EqMagElectricField*>    fEquations;
    std::vector<G4MagIntegratorStepper*>  fSteppers;
    std::vector<G4VIntegrationDriver*>   fDrivers;
    std::vector<G4ChordFinder*>           fChordFinders;
};

#endif 