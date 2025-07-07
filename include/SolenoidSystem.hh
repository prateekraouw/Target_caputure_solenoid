#ifndef SOLENOIDSYSTEM_HH
#define SOLENOIDSYSTEM_HH

#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4ClassicalRK4.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4ChordFinder.hh"
#include <vector>

// Structure to hold complete physical solenoid parameters
struct SolenoidParameters {
    G4double Rc;      // Coil radius [m] - center radius of windings
    G4double Zc;      // Coil center position [m] - axial position
    G4double DR;      // Radial extent [m] - thickness of coil windings
    G4double DZ;      // Axial extent [m] - length of coil
    G4int NR;         // Number of turns radially
    G4int NZ;         // Number of turns axially  
    G4double current; // Current [A] - current through windings
};
// Forward declaration
class SolenoidSystem;

class SolenoidFringeField : public G4MagneticField
{
private:
    G4double fBz0;           // Central field strength
    G4double fRadius;        // Solenoid radius
    G4double fLength;        // Solenoid length
    G4double fZCenter;       // Z position of solenoid center
    
public:
    SolenoidFringeField(G4double Bz0, G4double radius, G4double length, G4double zCenter);
    
    void GetFieldValue(const G4double point[4], G4double* field) const override;
    
private:
    G4double FringeFieldFactor(G4double z) const;
    void CalculateFringeField(G4double rho, G4double z, G4double* field) const;
};



class SolenoidSystem
{
public:
SolenoidSystem();
    ~SolenoidSystem();
    
    // Main function - takes direct physical parameters for single solenoid
    void CreateRampUpSolenoids(G4double Rc,       // Coil radius [m]
                              G4double Zc,        // Coil center position [m] 
                              G4double DR,        // Radial extent [m]
                              G4double DZ,        // Axial extent [m]
                              G4int NR,           // Number of turns radially
                              G4int NZ,           // Number of turns axially
                              G4double current,   // Current [A]
                              G4LogicalVolume* motherVolume);
    
    // Get volumes with magnetic fields
    std::vector<G4LogicalVolume*> GetMagFieldVolumes() const;
    
    // Print system information
    void PrintSystemInfo() const;

private:
// Materials
    G4Material* fAirMaterial;
    G4Material* fCopperMaterial;
    G4Material* fInsulatorMaterial;
    
    // Vectors to track field-related objects for proper cleanup
    std::vector<G4MagneticField*> fMagneticFields;  // Changed to base class
    std::vector<G4FieldManager*> fFieldManagers;
    std::vector<G4Mag_UsualEqRhs*> fEquations;
    std::vector<G4ClassicalRK4*> fSteppers;
    std::vector<G4MagInt_Driver*> fDrivers;
    std::vector<G4ChordFinder*> fChordFinders;
    
    // System data
    std::vector<G4LogicalVolume*> fMagFieldVolumes;
    std::vector<G4double> fFieldStrengths;
    std::vector<SolenoidParameters> fSolenoidParameters;
    
    // Private methods
    void InitializeMaterials();
    void CleanupFieldObjects();
    
    // Complete solenoid implementation methods
    G4double CalculateMagneticField(G4double Rc, G4double DZ, G4int NR, G4int NZ, G4double current);
    G4double CalculateRequiredCurrent(G4double Rc, G4double DZ, G4int NR, G4int NZ, G4double targetField);
    
    G4LogicalVolume* CreateAirSolenoid(const SolenoidParameters& params,
                                      G4LogicalVolume* motherVolume);
    
    void CreateCoilStructure(G4int solenoidID, const SolenoidParameters& params,
                            G4LogicalVolume* motherVolume);
    
    G4Material* CreateCoilMaterial(G4int NR, G4int NZ);
    
    void SetAirSolenoidVisualization(G4LogicalVolume* solenoidLogical, 
                                   G4int solenoidID, G4double radius);
    
    // Field manager creation
    void CreateFieldManagers();
};

#endif // SOLENOIDSYSTEM_HH