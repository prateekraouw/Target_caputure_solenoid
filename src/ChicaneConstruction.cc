#include "ChicaneConstruction.hh"
#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4ClassicalRK4.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4TransportationManager.hh"

ChicaneConstruction::ChicaneConstruction()
: G4VUserDetectorConstruction(),
  fDetectorConstruction(nullptr),
  fWorldLogical(nullptr),
  fWorldPhysical(nullptr),
  fMagnet1Volume(nullptr),
  fMagnet2Volume(nullptr),
  fMagnet3Volume(nullptr),
  fMagnet4Volume(nullptr),
  fMagField1(nullptr),
  fMagField2(nullptr),
  fMagField3(nullptr),
  fMagField4(nullptr),
  fFieldManager1(nullptr),
  fFieldManager2(nullptr),
  fFieldManager3(nullptr),
  fFieldManager4(nullptr)
{
  // Chicane parameters
  fMagnetLength = 20*cm;      // Length of each magnet along beam direction
  fMagnetWidth = 50*cm;       // Width perpendicular to beam
  fMagnetHeight = 50*cm;      // Height perpendicular to beam
  fMagnetSeparation = 5*cm;   // Drift space between magnets
  fFieldStrength = 0.5*CLHEP::tesla; // Magnetic field strength - 5 Tesla
  
  G4cout << "\n=== ChicaneConstruction Initialized ===" << G4endl;
  G4cout << "Chicane parameters:" << G4endl;
  G4cout << "  Magnet length: " << fMagnetLength/cm << " cm" << G4endl;
  G4cout << "  Magnet width: " << fMagnetWidth/cm << " cm" << G4endl;
  G4cout << "  Magnet height: " << fMagnetHeight/cm << " cm" << G4endl;
  G4cout << "  Magnet separation: " << fMagnetSeparation/cm << " cm" << G4endl;
  G4cout << "  Field strength: " << fFieldStrength/tesla << " Tesla" << G4endl;
  G4cout << "======================================\n" << G4endl;
}

ChicaneConstruction::~ChicaneConstruction()
{
  // Clean up magnetic fields
  delete fMagField1;
  delete fMagField2;
  delete fMagField3;
  delete fMagField4;
  // Note: field managers will be deleted by Geant4
  // Don't delete DetectorConstruction as it's managed elsewhere
  
  G4cout << "ChicaneConstruction destroyed and cleaned up" << G4endl;
}

G4VPhysicalVolume* ChicaneConstruction::Construct()
{
  G4cout << "\n=== ChicaneConstruction::Construct() ===" << G4endl;
  
  // Create DetectorConstruction to get the world volume
  fDetectorConstruction = new DetectorConstruction();
  fWorldPhysical = fDetectorConstruction->Construct();
  
  // Get the world logical volume from DetectorConstruction
  fWorldLogical = fWorldPhysical->GetLogicalVolume();
  
  G4cout << "Successfully obtained world volume from DetectorConstruction" << G4endl;
  G4cout << "World volume name: " << fWorldLogical->GetName() << G4endl;
  
  // Create each chicane magnet individually in the existing world
  G4cout << "\nCreating chicane magnets in DetectorConstruction world..." << G4endl;
  CreateMagnet1();
  CreateMagnet2();
  CreateMagnet3();
  CreateMagnet4();
  
  G4cout << "All chicane magnets successfully added to DetectorConstruction world" << G4endl;
  G4cout << "==========================================\n" << G4endl;
  
  // Return the world physical volume from DetectorConstruction
  return fWorldPhysical;
}

void ChicaneConstruction::CreateMagnet1()
{
  G4NistManager* nist = G4NistManager::Instance();
  G4Material* magnet_mat = nist->FindOrBuildMaterial("G4_AIR");
  
  // Calculate chicane position - place it after detector 2
  // Assuming detector 2 is around 300 cm from origin based on your previous code
  G4double chicane_start_z = 550*cm;  // Start chicane at 350 cm
  G4double start_z = chicane_start_z;
  
  // Magnet 1 position
  G4double magnet1_z = start_z + fMagnetLength/2.0;
  
  // Create magnet 1 solid
  G4Box* solidMagnet1 = 
    new G4Box("ChicaneMagnet1",
              0.5*fMagnetWidth,
              0.5*fMagnetHeight,
              0.5*fMagnetLength);
  
  // Create logical volume for magnet 1
  fMagnet1Volume = 
    new G4LogicalVolume(solidMagnet1, magnet_mat, "ChicaneMagnet1");
  
  // Place magnet 1 in the DetectorConstruction world
  new G4PVPlacement(nullptr,                // no rotation
                    G4ThreeVector(0, 0, magnet1_z), // position
                    fMagnet1Volume,         // its logical volume
                    "ChicaneMagnet1",       // its name
                    fWorldLogical,          // DetectorConstruction world volume
                    false,                  // no boolean operation
                    0,                      // copy number
                    false);                 // no overlap checking for field volumes
  
  // Visualization - Red for +X field
  G4VisAttributes* magnet1_vis_att = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0, 0.6));
  magnet1_vis_att->SetVisibility(true);
  magnet1_vis_att->SetForceSolid(true);
  fMagnet1Volume->SetVisAttributes(magnet1_vis_att);
  
  G4cout << "  Created chicane magnet 1 at z = " << magnet1_z/cm 
         << " cm with +X field (5.0 T)" << G4endl;
}

void ChicaneConstruction::CreateMagnet2()
{
  G4NistManager* nist = G4NistManager::Instance();
  G4Material* magnet_mat = nist->FindOrBuildMaterial("G4_AIR");
  
  // Calculate chicane position
  G4double chicane_start_z = 550*cm;
  G4double start_z = chicane_start_z;
  
  // Magnet 2 position
  G4double magnet2_z = start_z + fMagnetLength + fMagnetSeparation + fMagnetLength/2.0;
  
  // Create magnet 2 solid
  G4Box* solidMagnet2 = 
    new G4Box("ChicaneMagnet2",
              0.5*fMagnetWidth,
              0.5*fMagnetHeight,
              0.5*fMagnetLength);
  
  // Create logical volume for magnet 2
  fMagnet2Volume = 
    new G4LogicalVolume(solidMagnet2, magnet_mat, "ChicaneMagnet2");
  
  // Place magnet 2 in the DetectorConstruction world
  new G4PVPlacement(nullptr,                // no rotation
                    G4ThreeVector(0, 0, magnet2_z), // position
                    fMagnet2Volume,         // its logical volume
                    "ChicaneMagnet2",       // its name
                    fWorldLogical,          // DetectorConstruction world volume
                    false,                  // no boolean operation
                    1,                      // copy number
                    false);                 // no overlap checking for field volumes
  
  // Visualization - Blue for -X field
  G4VisAttributes* magnet2_vis_att = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0, 0.6));
  magnet2_vis_att->SetVisibility(true);
  magnet2_vis_att->SetForceSolid(true);
  fMagnet2Volume->SetVisAttributes(magnet2_vis_att);
  
  G4cout << "  Created chicane magnet 2 at z = " << magnet2_z/cm 
         << " cm with -X field (5.0 T)" << G4endl;
}

void ChicaneConstruction::CreateMagnet3()
{
  G4NistManager* nist = G4NistManager::Instance();
  G4Material* magnet_mat = nist->FindOrBuildMaterial("G4_AIR");
  
  // Calculate chicane position
  G4double chicane_start_z = 550*cm;
  G4double start_z = chicane_start_z;
  
  // Magnet 3 position
  G4double magnet3_z = start_z + 2*fMagnetLength + 2*fMagnetSeparation + fMagnetLength/2.0;
  
  // Create magnet 3 solid
  G4Box* solidMagnet3 = 
    new G4Box("ChicaneMagnet3",
              0.5*fMagnetWidth,
              0.5*fMagnetHeight,
              0.5*fMagnetLength);
  
  // Create logical volume for magnet 3
  fMagnet3Volume = 
    new G4LogicalVolume(solidMagnet3, magnet_mat, "ChicaneMagnet3");
  
  // Place magnet 3 in the DetectorConstruction world
  new G4PVPlacement(nullptr,                // no rotation
                    G4ThreeVector(0, 0, magnet3_z), // position
                    fMagnet3Volume,         // its logical volume
                    "ChicaneMagnet3",       // its name
                    fWorldLogical,          // DetectorConstruction world volume
                    false,                  // no boolean operation
                    2,                      // copy number
                    false);                 // no overlap checking for field volumes
  
  // Visualization - Blue for -X field
  G4VisAttributes* magnet3_vis_att = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0, 0.6));
  magnet3_vis_att->SetVisibility(true);
  magnet3_vis_att->SetForceSolid(true);
  fMagnet3Volume->SetVisAttributes(magnet3_vis_att);
  
  G4cout << "  Created chicane magnet 3 at z = " << magnet3_z/cm 
         << " cm with -X field (5.0 T)" << G4endl;
}

void ChicaneConstruction::CreateMagnet4()
{
  G4NistManager* nist = G4NistManager::Instance();
  G4Material* magnet_mat = nist->FindOrBuildMaterial("G4_AIR");
  
  // Calculate chicane position
  G4double chicane_start_z = 550*cm;
  G4double start_z = chicane_start_z;
  
  // Magnet 4 position
  G4double magnet4_z = start_z + 3*fMagnetLength + 3*fMagnetSeparation + fMagnetLength/2.0;
  
  // Create magnet 4 solid
  G4Box* solidMagnet4 = 
    new G4Box("ChicaneMagnet4",
              0.5*fMagnetWidth,
              0.5*fMagnetHeight,
              0.5*fMagnetLength);
  
  // Create logical volume for magnet 4
  fMagnet4Volume = 
    new G4LogicalVolume(solidMagnet4, magnet_mat, "ChicaneMagnet4");
  
  // Place magnet 4 in the DetectorConstruction world
  new G4PVPlacement(nullptr,                // no rotation
                    G4ThreeVector(0, 0, magnet4_z), // position
                    fMagnet4Volume,         // its logical volume
                    "ChicaneMagnet4",       // its name
                    fWorldLogical,          // DetectorConstruction world volume
                    false,                  // no boolean operation
                    3,                      // copy number
                    false);                 // no overlap checking for field volumes
  
  // Visualization - Red for +X field
  G4VisAttributes* magnet4_vis_att = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0, 0.6));
  magnet4_vis_att->SetVisibility(true);
  magnet4_vis_att->SetForceSolid(true);
  fMagnet4Volume->SetVisAttributes(magnet4_vis_att);
  
  G4cout << "  Created chicane magnet 4 at z = " << magnet4_z/cm 
         << " cm with +X field (5.0 T)" << G4endl;
}

void ChicaneConstruction::ConstructSDandField()
{
  G4cout << "\n=== ChicaneConstruction::ConstructSDandField() ===" << G4endl;
  
  // First, call DetectorConstruction's ConstructSDandField if it exists
  if (fDetectorConstruction) {
    G4cout << "Calling DetectorConstruction::ConstructSDandField()..." << G4endl;
    fDetectorConstruction->ConstructSDandField();
  }
  
  if (!fMagnet1Volume || !fMagnet2Volume || !fMagnet3Volume || !fMagnet4Volume) {
    G4cerr << "Error: Not all chicane magnet volumes created!" << G4endl;
    return;
  }
  
  // Setup magnetic fields for each magnet individually
  G4cout << "Setting up chicane magnetic fields..." << G4endl;
  SetupMagnet1Field();
  SetupMagnet2Field();
  SetupMagnet3Field();
  SetupMagnet4Field();
  
  G4cout << "\n-----------------------------------------------------------" << G4endl;
  G4cout << " Magnetic Chicane Setup Complete!" << G4endl;
  G4cout << " Number of magnets: 4 individual dipole magnets" << G4endl;
  G4cout << " Field strength: " << fFieldStrength/tesla << " Tesla" << G4endl;
  G4cout << " Magnet length: " << fMagnetLength/cm << " cm" << G4endl;
  G4cout << " Magnet separation: " << fMagnetSeparation/cm << " cm" << G4endl;
  G4cout << " Field pattern: +X, -X, -X, +X (standard chicane for Z-axis beam)" << G4endl;
  G4cout << " Chicane position: Starting at z = 350 cm" << G4endl;
  G4cout << " Total chicane length: " << (4*fMagnetLength + 3*fMagnetSeparation)/cm << " cm" << G4endl;
  G4cout << "-----------------------------------------------------------\n" << G4endl;
}

void ChicaneConstruction::SetupMagnet1Field()
{
  // Magnet 1: +X field (5.0 Tesla)
  G4ThreeVector fieldValue1 = G4ThreeVector(+1.0 * fFieldStrength, 0.0, 0.0);
  fMagField1 = new ChicaneMagField(fieldValue1);
  
  // Create field manager for magnet 1
  fFieldManager1 = new G4FieldManager(fMagField1);
  
  // Create equation of motion
  G4Mag_UsualEqRhs* equation1 = new G4Mag_UsualEqRhs(fMagField1);
  
  // Create stepper
  G4ClassicalRK4* stepper1 = new G4ClassicalRK4(equation1);
  
  // Create chord finder
  G4double minStep = 0.001*mm;  // Smaller step for high field accuracy
  G4MagInt_Driver* driver1 = new G4MagInt_Driver(minStep, stepper1, stepper1->GetNumberOfVariables());
  G4ChordFinder* chordFinder1 = new G4ChordFinder(driver1);
  
  // Set field manager parameters - higher precision for 5T field
  fFieldManager1->SetChordFinder(chordFinder1);
  fFieldManager1->SetDetectorField(fMagField1);
  fFieldManager1->SetDeltaOneStep(0.001*mm);
  fFieldManager1->SetDeltaIntersection(0.001*mm);
  
  // Apply field manager to magnet 1
  fMagnet1Volume->SetFieldManager(fFieldManager1, true);
  
  G4cout << "  Applied magnetic field (+5.0, 0.0, 0.0) Tesla to magnet 1" << G4endl;
}

void ChicaneConstruction::SetupMagnet2Field()
{
  // Magnet 2: -X field (5.0 Tesla)
  G4ThreeVector fieldValue2 = G4ThreeVector(-1.0 * fFieldStrength, 0.0, 0.0);
  fMagField2 = new ChicaneMagField(fieldValue2);
  
  // Create field manager for magnet 2
  fFieldManager2 = new G4FieldManager(fMagField2);
  
  // Create equation of motion
  G4Mag_UsualEqRhs* equation2 = new G4Mag_UsualEqRhs(fMagField2);
  
  // Create stepper
  G4ClassicalRK4* stepper2 = new G4ClassicalRK4(equation2);
  
  // Create chord finder
  G4double minStep = 0.001*mm;  // Smaller step for high field accuracy
  G4MagInt_Driver* driver2 = new G4MagInt_Driver(minStep, stepper2, stepper2->GetNumberOfVariables());
  G4ChordFinder* chordFinder2 = new G4ChordFinder(driver2);
  
  // Set field manager parameters - higher precision for 5T field
  fFieldManager2->SetChordFinder(chordFinder2);
  fFieldManager2->SetDetectorField(fMagField2);
  fFieldManager2->SetDeltaOneStep(0.001*mm);
  fFieldManager2->SetDeltaIntersection(0.001*mm);
  
  // Apply field manager to magnet 2
  fMagnet2Volume->SetFieldManager(fFieldManager2, true);
  
  G4cout << "  Applied magnetic field (-5.0, 0.0, 0.0) Tesla to magnet 2" << G4endl;
}

void ChicaneConstruction::SetupMagnet3Field()
{
  // Magnet 3: -X field (5.0 Tesla)
  G4ThreeVector fieldValue3 = G4ThreeVector(-1.0 * fFieldStrength, 0.0, 0.0);
  fMagField3 = new ChicaneMagField(fieldValue3);
  
  // Create field manager for magnet 3
  fFieldManager3 = new G4FieldManager(fMagField3);
  
  // Create equation of motion
  G4Mag_UsualEqRhs* equation3 = new G4Mag_UsualEqRhs(fMagField3);
  
  // Create stepper
  G4ClassicalRK4* stepper3 = new G4ClassicalRK4(equation3);
  
  // Create chord finder
  G4double minStep = 0.001*mm;  // Smaller step for high field accuracy
  G4MagInt_Driver* driver3 = new G4MagInt_Driver(minStep, stepper3, stepper3->GetNumberOfVariables());
  G4ChordFinder* chordFinder3 = new G4ChordFinder(driver3);
  
  // Set field manager parameters - higher precision for 5T field
  fFieldManager3->SetChordFinder(chordFinder3);
  fFieldManager3->SetDetectorField(fMagField3);
  fFieldManager3->SetDeltaOneStep(0.001*mm);
  fFieldManager3->SetDeltaIntersection(0.001*mm);
  
  // Apply field manager to magnet 3
  fMagnet3Volume->SetFieldManager(fFieldManager3, true);
  
  G4cout << "  Applied magnetic field (-5.0, 0.0, 0.0) Tesla to magnet 3" << G4endl;
}

void ChicaneConstruction::SetupMagnet4Field()
{
  // Magnet 4: +X field (5.0 Tesla)
  G4ThreeVector fieldValue4 = G4ThreeVector(+1.0 * fFieldStrength, 0.0, 0.0);
  fMagField4 = new ChicaneMagField(fieldValue4);
  
  // Create field manager for magnet 4
  fFieldManager4 = new G4FieldManager(fMagField4);
  
  // Create equation of motion
  G4Mag_UsualEqRhs* equation4 = new G4Mag_UsualEqRhs(fMagField4);
  
  // Create stepper
  G4ClassicalRK4* stepper4 = new G4ClassicalRK4(equation4);
  
  // Create chord finder
  G4double minStep = 0.001*mm;  // Smaller step for high field accuracy
  G4MagInt_Driver* driver4 = new G4MagInt_Driver(minStep, stepper4, stepper4->GetNumberOfVariables());
  G4ChordFinder* chordFinder4 = new G4ChordFinder(driver4);
  
  // Set field manager parameters - higher precision for 5T field
  fFieldManager4->SetChordFinder(chordFinder4);
  fFieldManager4->SetDetectorField(fMagField4);
  fFieldManager4->SetDeltaOneStep(0.001*mm);
  fFieldManager4->SetDeltaIntersection(0.001*mm);
  
  // Apply field manager to magnet 4
  fMagnet4Volume->SetFieldManager(fFieldManager4, true);
  
  G4cout << "  Applied magnetic field (+5.0, 0.0, 0.0) Tesla to magnet 4" << G4endl;
}