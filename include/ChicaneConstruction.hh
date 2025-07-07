#ifndef ChicaneConstruction_h
#define ChicaneConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4MagneticField.hh"
#include "G4FieldManager.hh"
#include "G4Box.hh"
#include "G4SystemOfUnits.hh"

// Forward declaration
class DetectorConstruction;

/**
 * @brief Uniform magnetic field class for chicane magnets
 * 
 * This class implements a uniform magnetic field that can be used
 * for chicane dipole magnets. The field is constant throughout
 * the volume where it is applied.
 */
class ChicaneMagField : public G4MagneticField
{
public:
  /**
   * @brief Constructor
   * @param fieldVector The magnetic field vector (Bx, By, Bz)
   */
  ChicaneMagField(const G4ThreeVector& fieldVector) 
  : G4MagneticField(), fFieldValue(fieldVector)
  {
    // Print field information when created
    G4cout << "ChicaneMagField created with field: (" 
           << fFieldValue.x()/tesla << ", " 
           << fFieldValue.y()/tesla << ", " 
           << fFieldValue.z()/tesla << ") Tesla" << G4endl;
  }
  
  /**
   * @brief Destructor
   */
  virtual ~ChicaneMagField() {}
  
  /**
   * @brief Get the magnetic field value at a given point
   * @param point The 4D space-time point [x, y, z, t]
   * @param field The output field array [Bx, By, Bz, Ex, Ey, Ez]
   */
  virtual void GetFieldValue(const G4double[4], G4double* field) const override
  {
    // Set magnetic field components
    field[0] = fFieldValue.x();  // Bx
    field[1] = fFieldValue.y();  // By
    field[2] = fFieldValue.z();  // Bz
    
    // Set electric field components to zero (pure magnetic field)
    field[3] = 0.0;  // Ex
    field[4] = 0.0;  // Ey
    field[5] = 0.0;  // Ez
  }
  
  /**
   * @brief Get the current field value
   * @return The magnetic field vector
   */
  G4ThreeVector GetFieldValue() const { return fFieldValue; }
  
  /**
   * @brief Set a new field value
   * @param fieldValue The new magnetic field vector
   */
  void SetFieldValue(const G4ThreeVector& fieldValue) { fFieldValue = fieldValue; }
  
  /**
   * @brief Get the field strength (magnitude)
   * @return The magnitude of the magnetic field
   */
  G4double GetFieldStrength() const { return fFieldValue.mag(); }
  
  /**
   * @brief Check if this is a pure magnetic field (no electric component)
   * @return Always false for pure magnetic field (doesn't change particle energy)
   */
  virtual G4bool DoesFieldChangeEnergy() const override { return false; }

private:
  G4ThreeVector fFieldValue;  ///< The magnetic field vector (Bx, By, Bz)
};

/**
 * @brief Chicane construction class that uses DetectorConstruction world volume
 * 
 * This class creates a 4-magnet chicane system with 5 Tesla dipole magnets
 * and integrates it with the existing DetectorConstruction geometry.
 */
class ChicaneConstruction : public G4VUserDetectorConstruction
{
public:
  ChicaneConstruction();
  ~ChicaneConstruction();
  
  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField();
  
  // Getter methods for chicane parameters
  G4double GetMagnetLength() const { return fMagnetLength; }
  G4double GetMagnetSeparation() const { return fMagnetSeparation; }
  G4double GetFieldStrength() const { return fFieldStrength; }
  
private:
  // Use DetectorConstruction to get the world volume
  DetectorConstruction* fDetectorConstruction;
  G4LogicalVolume* fWorldLogical;
  G4VPhysicalVolume* fWorldPhysical;
  
  // Individual logical volumes for each magnet
  G4LogicalVolume* fMagnet1Volume;
  G4LogicalVolume* fMagnet2Volume;
  G4LogicalVolume* fMagnet3Volume;
  G4LogicalVolume* fMagnet4Volume;
  
  // Individual field-related member variables
  ChicaneMagField* fMagField1;
  ChicaneMagField* fMagField2;
  ChicaneMagField* fMagField3;
  ChicaneMagField* fMagField4;
  
  G4FieldManager* fFieldManager1;
  G4FieldManager* fFieldManager2;
  G4FieldManager* fFieldManager3;
  G4FieldManager* fFieldManager4;
  
  // Chicane geometry parameters
  G4double fMagnetLength;
  G4double fMagnetWidth;
  G4double fMagnetHeight;
  G4double fMagnetSeparation;
  G4double fFieldStrength;
  
  // Individual magnet creation methods
  void CreateMagnet1();
  void CreateMagnet2();
  void CreateMagnet3();
  void CreateMagnet4();
  
  // Individual field setup methods
  void SetupMagnet1Field();
  void SetupMagnet2Field();
  void SetupMagnet3Field();
  void SetupMagnet4Field();
};

#endif