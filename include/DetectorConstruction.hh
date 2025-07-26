#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4MagneticField.hh"
#include "G4FieldManager.hh"
#include "SolenoidSystem.hh"
#include "MomentumChicane.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class ElectricFieldSetup;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction(G4double g1 = 0.0, G4double g2 = 0.0);
    ~DetectorConstruction() override;
    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

    // Original getters
    G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }
    G4LogicalVolume* GetDetector1Volume() const { return fDetector1Volume; }
    G4LogicalVolume* GetDetector2Volume() const { return fDetector2Volume; }
    G4LogicalVolume* GetDetector3Volume() const { return fDetector3Volume; }
    G4LogicalVolume* GetDetector4Volume() const { return fDetector4Volume; }
    G4LogicalVolume* GetRFCavityVolume() const { return fRFCavityVolume; }

    // Original position getters
    G4ThreeVector GetDetector1Position() const { return fDetector1Position; }
    G4ThreeVector GetDetector2Position() const { return fDetector2Position; }
    G4ThreeVector GetDetector3Position() const { return fDetector3Position; }
    G4ThreeVector GetDetector4Position() const { return fDetector4Position; }

      // New system getters - Updated for 23 solenoid systems
    SolenoidSystem* GetSolenoidSystem1() const { return fSolenoidSystem1; }
    SolenoidSystem* GetSolenoidSystem2() const { return fSolenoidSystem2; }
    SolenoidSystem* GetSolenoidSystem3() const { return fSolenoidSystem3; }
    SolenoidSystem* GetSolenoidSystem4() const { return fSolenoidSystem4; }
    SolenoidSystem* GetSolenoidSystem5() const { return fSolenoidSystem5; }
    SolenoidSystem* GetSolenoidSystem6() const { return fSolenoidSystem6; }
    SolenoidSystem* GetSolenoidSystem7() const { return fSolenoidSystem7; }
    SolenoidSystem* GetSolenoidSystem8() const { return fSolenoidSystem8; }
    SolenoidSystem* GetSolenoidSystem9() const { return fSolenoidSystem9; }
    SolenoidSystem* GetSolenoidSystem10() const { return fSolenoidSystem10; }
    SolenoidSystem* GetSolenoidSystem11() const { return fSolenoidSystem11; }
    SolenoidSystem* GetSolenoidSystem12() const { return fSolenoidSystem12; }
    SolenoidSystem* GetSolenoidSystem13() const { return fSolenoidSystem13; }
    SolenoidSystem* GetSolenoidSystem14() const { return fSolenoidSystem14; }
    SolenoidSystem* GetSolenoidSystem15() const { return fSolenoidSystem15; }
    SolenoidSystem* GetSolenoidSystem16() const { return fSolenoidSystem16; }
    SolenoidSystem* GetSolenoidSystem17() const { return fSolenoidSystem17; }
    SolenoidSystem* GetSolenoidSystem18() const { return fSolenoidSystem18; }
    SolenoidSystem* GetSolenoidSystem19() const { return fSolenoidSystem19; }
    SolenoidSystem* GetSolenoidSystem20() const { return fSolenoidSystem20; }
    SolenoidSystem* GetSolenoidSystem21() const { return fSolenoidSystem21; }
    SolenoidSystem* GetSolenoidSystem22() const { return fSolenoidSystem22; }
    SolenoidSystem* GetSolenoidSystem23() const { return fSolenoidSystem23; }

    MomentumChicane* GetMomentumChicane() const { return fMomentumChicane; }
    G4LogicalVolume* GetPrimaryCollimatorVolume() const { return fPrimaryCollimatorVolume; }
    G4LogicalVolume* GetSecondaryCollimatorVolume() const { return fSecondaryCollimatorVolume; }

    // Analysis methods
    void AnalyzeCompleteSystem();

private:
    // New advanced systems
    SolenoidSystem* fSolenoidSystem1;
    SolenoidSystem* fSolenoidSystem2;
    SolenoidSystem* fSolenoidSystem3;
    SolenoidSystem* fSolenoidSystem4;
    SolenoidSystem* fSolenoidSystem5;
    SolenoidSystem* fSolenoidSystem6;
    SolenoidSystem* fSolenoidSystem7;
    SolenoidSystem* fSolenoidSystem8;
    SolenoidSystem* fSolenoidSystem9;
    SolenoidSystem* fSolenoidSystem10;
    SolenoidSystem* fSolenoidSystem11;
    SolenoidSystem* fSolenoidSystem12;
    SolenoidSystem* fSolenoidSystem13;
    SolenoidSystem* fSolenoidSystem14;
    SolenoidSystem* fSolenoidSystem15;
    SolenoidSystem* fSolenoidSystem16;
    SolenoidSystem* fSolenoidSystem17;
    SolenoidSystem* fSolenoidSystem18;
    SolenoidSystem* fSolenoidSystem19;
    SolenoidSystem* fSolenoidSystem20;
    SolenoidSystem* fSolenoidSystem21;
    SolenoidSystem* fSolenoidSystem22;
    SolenoidSystem* fSolenoidSystem23;
    MomentumChicane* fMomentumChicane;

    // Original members (unchanged)
    G4LogicalVolume* fWorldLogical;
    G4LogicalVolume* fScoringVolume;
    G4LogicalVolume* fBore;
    G4LogicalVolume* fBore_2;
    G4LogicalVolume* fBore_3;
    G4LogicalVolume* fDetector1Volume;
    G4LogicalVolume* fDetector2Volume;
    G4LogicalVolume* fDetector3Volume;
    G4LogicalVolume* fDetector4Volume;
    G4LogicalVolume* fRFCavityVolume;
    G4LogicalVolume* fInitialSolenoidVolume;
    G4MagneticField* fInitialSolenoidField;
    G4FieldManager* fInitialSolenoidFieldManager;

    // New collimator volumes
    G4LogicalVolume* fPrimaryCollimatorVolume;
    G4LogicalVolume* fSecondaryCollimatorVolume;

    // Original detector positions
    G4ThreeVector fTungstenAperturePosition;
    G4ThreeVector fDetector1Position;
    G4ThreeVector fDetector2Position;
    G4ThreeVector fDetector3Position;
    G4ThreeVector fDetector4Position;

    G4double fGap1, fGap2;

    // New methods
    void CreateInitialSolenoid(G4double start, G4double len);
    void CreateTaperedSolenoid(G4double start, G4double len);
    void CreateCollimationSystem();
    void CreatePrimaryCollimator();
    void CreateSecondaryCollimator();
};

// Keep original uniform magnetic field class
class UniformMagField : public G4MagneticField
{
public:
    UniformMagField(const G4ThreeVector& fieldVector) : fFieldValue(fieldVector) {}
    virtual ~UniformMagField() {}

    virtual void GetFieldValue(const G4double[4], G4double* field) const override {
        field[0] = fFieldValue.x();
        field[1] = fFieldValue.y();
        field[2] = fFieldValue.z();
        field[3] = 0.0;
        field[4] = 0.0;
        field[5] = 0.0;
    }

private:
    G4ThreeVector fFieldValue;
};

#endif
