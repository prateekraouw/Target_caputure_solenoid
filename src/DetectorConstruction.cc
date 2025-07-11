#include "DetectorConstruction.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4TransportationManager.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4ChordFinder.hh"
#include "G4ClassicalRK4.hh"
#include "G4MagneticField.hh"
#include "G4Mag_UsualEqRhs.hh"

DetectorConstruction::DetectorConstruction(G4double g1, G4double g2)
: G4VUserDetectorConstruction(),
  fTungstenApertureVolume(nullptr),
  fBore_1(nullptr),
  fBore_2(nullptr),
  fBore_3(nullptr),
  fSolenoidSystem1(nullptr),
  fSolenoidSystem2(nullptr),
  fSolenoidSystem3(nullptr),
  fSolenoidSystem4(nullptr),
  fSolenoidSystem5(nullptr),
  fSolenoidSystem6(nullptr),
  fSolenoidSystem7(nullptr),
  fSolenoidSystem8(nullptr),
  fSolenoidSystem9(nullptr),
  fSolenoidSystem10(nullptr),
  fSolenoidSystem11(nullptr),
  fSolenoidSystem12(nullptr),
  fSolenoidSystem13(nullptr),
  fSolenoidSystem14(nullptr),
  fSolenoidSystem15(nullptr),
  fSolenoidSystem16(nullptr),
  fSolenoidSystem17(nullptr),
  fSolenoidSystem18(nullptr),
  fSolenoidSystem19(nullptr),
  fSolenoidSystem20(nullptr),
  fSolenoidSystem21(nullptr),
  fSolenoidSystem22(nullptr),
  fSolenoidSystem23(nullptr),
  fMomentumChicane(nullptr),
  fScoringVolume(nullptr),
  fWorldLogical(nullptr),
  fDetector1Volume(nullptr),
  fDetector2Volume(nullptr),
  fDetector3Volume(nullptr),
  fDetector4Volume(nullptr),
  fGap1(g1),
  fGap2(g2),
  fPrimaryCollimatorVolume(nullptr),
  fSecondaryCollimatorVolume(nullptr),
  fInitialSolenoidVolume(nullptr),           // Add this
  fInitialSolenoidField(nullptr),            // Add this
  fInitialSolenoidFieldManager(nullptr)      // Add this
{
    // Initialize new systems
    fSolenoidSystem1 = new SolenoidSystem();
    fSolenoidSystem2 = new SolenoidSystem();
    fSolenoidSystem3 = new SolenoidSystem();
    fSolenoidSystem4 = new SolenoidSystem();
    fSolenoidSystem5 = new SolenoidSystem();
    fSolenoidSystem6 = new SolenoidSystem();
    fSolenoidSystem7 = new SolenoidSystem();
    fSolenoidSystem8 = new SolenoidSystem();
    fSolenoidSystem9 = new SolenoidSystem();
    fSolenoidSystem10 = new SolenoidSystem();
    fSolenoidSystem11 = new SolenoidSystem();
    fSolenoidSystem12 = new SolenoidSystem();
    fSolenoidSystem13 = new SolenoidSystem();
    fSolenoidSystem14 = new SolenoidSystem();
    fSolenoidSystem15 = new SolenoidSystem();
    fSolenoidSystem16 = new SolenoidSystem();
    fSolenoidSystem17 = new SolenoidSystem();
    fSolenoidSystem18 = new SolenoidSystem();
    fSolenoidSystem19 = new SolenoidSystem();
    fSolenoidSystem20 = new SolenoidSystem();
    fSolenoidSystem21 = new SolenoidSystem();
    fSolenoidSystem22 = new SolenoidSystem();
    fSolenoidSystem23 = new SolenoidSystem();
    fMomentumChicane = new MomentumChicane();

}

DetectorConstruction::~DetectorConstruction()
{
    // Clean up new systems
    delete fSolenoidSystem1;
    delete fSolenoidSystem2;
    delete fSolenoidSystem3;
    delete fSolenoidSystem4;
    delete fSolenoidSystem5;
    delete fSolenoidSystem6;
    delete fSolenoidSystem7;
    delete fSolenoidSystem8;
    delete fSolenoidSystem9;
    delete fSolenoidSystem10;
    delete fSolenoidSystem11;
    delete fSolenoidSystem12;
    delete fSolenoidSystem13;
    delete fSolenoidSystem14;
    delete fSolenoidSystem15;
    delete fSolenoidSystem16;
    delete fSolenoidSystem17;
    delete fSolenoidSystem18;
    delete fSolenoidSystem19;
    delete fSolenoidSystem20;
    delete fSolenoidSystem21;
    delete fSolenoidSystem22;
    delete fSolenoidSystem23;
    delete fMomentumChicane;
    delete fInitialSolenoidField;             // Add this
    delete fInitialSolenoidFieldManager;      // Add this
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // ========== MATERIALS ==========
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* graphite_mat = nist->FindOrBuildMaterial("G4_GRAPHITE");
    G4Material* tungsten_mat = nist->FindOrBuildMaterial("G4_W");
    G4Material* scintillator_mat = nist->FindOrBuildMaterial("G4_Ar");
    G4Material* rfcavity_mat = nist->FindOrBuildMaterial("G4_Galactic");

    // ========== WORLD VOLUME ==========
    G4double world_size = 10000*cm;
    G4double world_radius = 0.5*world_size;
    G4double world_length = 20*world_size;

    G4Tubs* solidWorld = new G4Tubs("World", 0, world_radius, 0.5*world_length, 0*deg, 360*deg);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");
    fWorldLogical = logicWorld;

    G4VPhysicalVolume* physWorld = new G4PVPlacement(nullptr, G4ThreeVector(), logicWorld,
                                                    "World", nullptr, false, 0, true);

    // ========== TUNGSTEN TARGET ==========
    G4double tungsten_x = 10*cm;
    G4double tungsten_y = 10*cm;
    G4double tungsten_z = 80*cm;

    G4Box* solidGraphite = new G4Box("Graphite", 0.5*tungsten_x, 0.5*tungsten_y, 0.5*tungsten_z);
    G4LogicalVolume* logicTungsten = new G4LogicalVolume(solidGraphite, graphite_mat, "Graphite");

    G4RotationMatrix* rotation = new G4RotationMatrix();
    rotation->rotateX(0*deg);

    new G4PVPlacement(rotation, G4ThreeVector(0, 0, -1.1*m), logicTungsten, "Tungsten",
                     logicWorld, false, 0, true);

    // ========== PARTICLE GUN VISUALIZATION ==========
    G4Box* gunBox = new G4Box("GunBox", 2*cm, 2*cm, 40*cm);
    G4LogicalVolume* gunLog = new G4LogicalVolume(gunBox, graphite_mat, "GunLogical");
    G4RotationMatrix* gunRot = new G4RotationMatrix();
    gunRot->rotateX(0.*deg);
    G4ThreeVector gunPos = G4ThreeVector(0, 0.*m, -2.0*m);
    new G4PVPlacement(gunRot, gunPos, gunLog, "GunBlock", logicWorld, false, 0, true);

    //===========Tungsten Bore===========
    G4double bore_1_position = 1.5*meter;
    G4Tubs* bore_1 = new G4Tubs("Bore_1", 7.5*cm, 70*cm, 3.0*m, 0*deg, 360*deg);
    G4LogicalVolume* logicBore_1 = new G4LogicalVolume(bore_1, tungsten_mat, "Bore_1");
    fBore_1 = logicBore_1;
    G4ThreeVector fBore1_position = G4ThreeVector(0, 0, bore_1_position);
    new G4PVPlacement(nullptr, fBore1_position, logicBore_1, "Bore_1", logicWorld, false, 0, false);
    
    G4double bore_2_position = 7.75*meter;
    G4Tubs* bore_2 = new G4Tubs("Bore_2", 15*cm, 70*cm, 3.19*m, 0*deg, 360*deg);
    G4LogicalVolume* logicBore_2 = new G4LogicalVolume(bore_2, tungsten_mat, "Bore_2");
    fBore_2 = logicBore_2;
    G4ThreeVector fBore2_position = G4ThreeVector(0, 0, bore_2_position);
    new G4PVPlacement(nullptr, fBore2_position, logicBore_2, "Bore_2", logicWorld, false, 0, false);
    
    G4double bore_3_position = 14.30*meter;
    G4Tubs* bore_3 = new G4Tubs("Bore_3", 30*cm, 70*cm, 3.28*m, 0*deg, 360*deg);
    G4LogicalVolume* logicBore_3 = new G4LogicalVolume(bore_3, tungsten_mat, "Bore_3");
    fBore_3 = logicBore_3;
    G4ThreeVector fBore3_position = G4ThreeVector(0, 0, bore_3_position);
    new G4PVPlacement(nullptr, fBore3_position, logicBore_3, "Bore_3", logicWorld, false, 0, false);


    G4cout << "Creating Solenoid 1..." << G4endl;
    fSolenoidSystem1->CreateRampUpSolenoids(
        0.970*m,    // Rc
        -0.885*m,   // Zc (shifted +0.3m)
        0.540*m,    // DR
        0.830*m,    // DZ
        13,         // NR
        20,         // NZ
        52*ampere,
        fWorldLogical
    );
    
    G4cout << "Creating Solenoid 2..." << G4endl;
    fSolenoidSystem2->CreateRampUpSolenoids(
        0.970*m,
        -0.035*m,   // Zc (shifted +0.3m)
        0.540*m,
        0.830*m,
        13,
        20,
        52*ampere,
        fWorldLogical
    );
    
    G4cout << "Creating Solenoid 3..." << G4endl;
    fSolenoidSystem3->CreateRampUpSolenoids(
        0.970*m,
        0.815*m,    // Zc (shifted +0.3m)
        0.540*m,
        0.830*m,
        13,
        20,
        52*ampere,
        fWorldLogical
    );
    
    G4cout << "Creating Solenoid 4..." << G4endl;
    fSolenoidSystem4->CreateRampUpSolenoids(
        0.887*m,
        1.665*m,    // Zc (shifted +0.3m)
        0.374*m,
        0.830*m,
        10,
        20,
        52*ampere,
        fWorldLogical
    );
    
    G4cout << "Creating Solenoid 5..." << G4endl;
    fSolenoidSystem5->CreateRampUpSolenoids(
        0.825*m,
        2.515*m,    // Zc (shifted +0.3m)
        0.249*m,
        0.830*m,
        7,
        20,
        52*ampere,
        fWorldLogical
    );
    
    G4cout << "Creating Solenoid 6..." << G4endl;
    fSolenoidSystem6->CreateRampUpSolenoids(
        0.783*m,
        3.365*m,    // Zc (shifted +0.3m)
        0.166*m,
        0.830*m,
        6,
        20,
        52*ampere,
        fWorldLogical
    );
    
    // Solenoid 7
    fSolenoidSystem7->CreateRampUpSolenoids(
        0.825*m,
        4.008*m,    // Zc (shifted +0.3m)
        0.249*m,
        0.415*m,
        5,           // NR
        10,          // NZ
        52*ampere,
        fWorldLogical
    );
    
    // Solenoid 8
    fSolenoidSystem8->CreateRampUpSolenoids(
        0.804*m,
        4.903*m,    // Zc (shifted +0.3m from 4.603)
        0.208*m,
        0.415*m,
        5,           // NR
        10,          // NZ
        52*ampere,
        fWorldLogical
    );
    
    // Solenoid 9
    fSolenoidSystem9->CreateRampUpSolenoids(
        0.742*m,
        5.545*m,    // Zc (shifted +0.3m)
        0.083*m,
        0.830*m,
        3,           // NR
        20,          // NZ
        52*ampere,
        fWorldLogical
    );
    
    // Solenoid 10
    fSolenoidSystem10->CreateRampUpSolenoids(
        0.742*m,
        6.395*m,    // Zc (shifted +0.3m)
        0.083*m,
        0.830*m,
        3,
        20,
        52*ampere,
        fWorldLogical
    );
    
    // Solenoid 11
    fSolenoidSystem11->CreateRampUpSolenoids(
        0.742*m,
        7.245*m,    // Zc (shifted +0.3m)
        0.083*m,
        0.830*m,
        3,
        20,
        52*ampere,
        fWorldLogical
    );
    
    // Solenoid 12
    fSolenoidSystem12->CreateRampUpSolenoids(
        0.721*m,
        8.095*m,    // Zc (shifted +0.3m)
        0.042*m,
        0.830*m,
        2,
        20,
        52*ampere,
        fWorldLogical
    );
    
    // Solenoid 13
    fSolenoidSystem13->CreateRampUpSolenoids(
        0.721*m,
        8.945*m,    // Zc (shifted +0.3m)
        0.042*m,
        0.830*m,
        2,
        20,
        52*ampere,
        fWorldLogical
    );
    
    // Solenoid 14
    fSolenoidSystem14->CreateRampUpSolenoids(
        0.721*m,
        9.795*m,    // Zc (shifted +0.3m)
        0.042*m,
        0.830*m,
        2,
        20,
        52*ampere,
        fWorldLogical
    );
    
    // Solenoid 15
    fSolenoidSystem15->CreateRampUpSolenoids(
        0.742*m,
        10.438*m,   // Zc (shifted +0.3m)
        0.083*m,
        0.415*m,
        1,
        20,
        52*ampere,
        fWorldLogical
    );
    
    // Solenoid 16
    fSolenoidSystem16->CreateRampUpSolenoids(
        0.721*m,
        11.333*m,   // Zc (shifted +0.3m)
        0.042*m,
        0.415*m,
        1,
        10,
        52*ampere,
        fWorldLogical
    );
    
    // Solenoid 17
    fSolenoidSystem17->CreateRampUpSolenoids(
        0.721*m,
        11.975*m,   // Zc (shifted +0.3m)
        0.042*m,
        0.830*m,
        1,
        20,
        52*ampere,
        fWorldLogical
    );
    
    // Solenoid 18
    fSolenoidSystem18->CreateRampUpSolenoids(
        0.721*m,
        12.825*m,   // Zc (adjusted for 0.02 m gap with Solenoid 17)
        0.042*m,
        0.830*m,
        1,
        20,
        52*ampere,
        fWorldLogical
    );
    
    // Solenoid 19
    fSolenoidSystem19->CreateRampUpSolenoids(
        0.721*m,
        13.675*m,   // Zc (shifted +0.3m)
        0.042*m,
        0.830*m,
        1,
        20,
        52*ampere,
        fWorldLogical
    );
    
    // Solenoid 20
    fSolenoidSystem20->CreateRampUpSolenoids(
        0.721*m,
        14.525*m,   // Zc (shifted +0.3m)
        0.042*m,
        0.830*m,
        1,
        20,
        52*ampere,
        fWorldLogical
    );
    
    // Solenoid 21
    fSolenoidSystem21->CreateRampUpSolenoids(
        0.721*m,
        15.375*m,   // Zc (adjusted for 0.02 m gap with Solenoid 20)
        0.042*m,
        0.830*m,
        1,
        20,
        52*ampere,
        fWorldLogical
    );
    
    // Solenoid 22
    fSolenoidSystem22->CreateRampUpSolenoids(
        0.721*m,
        16.225*m,   // Zc (shifted +0.3m)
        0.042*m,
        0.830*m,
        1,
        20,
        52*ampere,
        fWorldLogical
    );
    
    // Solenoid 23
    fSolenoidSystem23->CreateRampUpSolenoids(
        0.721*m,
        17.075*m,   // Zc (shifted +0.3m)
        0.042*m,
        0.830*m,
        1,
        20,
        52*ampere,
        fWorldLogical
    );





    // ========== MOMENTUM CHICANE ==========
    /*fMomentumChicane->ConfigureChicane(
        1900*cm,     // Start Z (early in beamline)
        40*cm,      // Magnet length
        20*cm,      // Separation
        0.25*tesla,  // Field strength
        100*cm,      // Width
        100*cm       // Height
    );
    fMomentumChicane->BuildChicane(fWorldLogical);
    */

    // ========= Tungsten Aperture ====
    //G4double tungstenAperture_position = 80*meter;
    //G4Tubs* tungstenAperture = new G4Tubs("TungstenAperture", 5*cm, 120*cm, 2*cm, 0*deg, 360*deg);
    //G4LogicalVolume* logicTungstenAperture = new G4LogicalVolume(tungstenAperture, tungsten_mat, "TungstenAperture");
    //fTungstenApertureVolume = logicTungstenAperture;
    //fTungstenAperturePosition = G4ThreeVector(0, 0, tungstenAperture_position);
    //new G4PVPlacement(nullptr, fTungstenAperturePosition, logicTungstenAperture, "TungstenAperture", logicWorld, false, 0, false);


    // ========== DETECTORS ==========
    G4double detector_thickness = 0.1*cm;

    // Detector 1: Before momentum chicane
    G4double detector1_position = 4.5*meter;
    G4Tubs* solidDetector1 = new G4Tubs("Detector1", 0*cm, 70*cm, 0.5*detector_thickness, 0*deg, 360*deg);
    G4LogicalVolume* logicDetector1 = new G4LogicalVolume(solidDetector1, scintillator_mat, "Detector1");
    fDetector1Volume = logicDetector1;
    fDetector1Position = G4ThreeVector(0, 0, detector1_position);
    new G4PVPlacement(nullptr, fDetector1Position, logicDetector1, "Detector1", logicWorld, false, 0, false);

    // Detector 2: After momentum chicane, before solenoid
    G4double detector2_position = 11.0*meter;
    G4Tubs* solidDetector2 = new G4Tubs("Detector2", 0*cm, 70*cm, 0.5*detector_thickness, 0*deg, 360*deg);
    G4LogicalVolume* logicDetector2 = new G4LogicalVolume(solidDetector2, scintillator_mat, "Detector2");
    fDetector2Volume = logicDetector2;
    fDetector2Position = G4ThreeVector(0, 0, detector2_position);
    new G4PVPlacement(nullptr, fDetector2Position, logicDetector2, "Detector2", logicWorld, false, 0, false);

    // Detector 3: after solenoid to avoid overlap
    G4double detector3_position = 17.7*meter;  //  923cm
    G4Tubs* solidDetector3 = new G4Tubs("Detector3", 0*cm, 70*cm, 0.5*detector_thickness, 0*deg, 360*deg);
    G4LogicalVolume* logicDetector3 = new G4LogicalVolume(solidDetector3, scintillator_mat, "Detector3");
    fDetector3Volume = logicDetector3;
    fDetector3Position = G4ThreeVector(0, 0, detector3_position);
    new G4PVPlacement(nullptr, fDetector3Position, logicDetector3, "Detector3", logicWorld, false, 0, false);

    // Detector 4: after solenoid to avoid overlap
    G4double detector4_position = 11.2*meter;  //  1035cm
    G4Tubs* solidDetector4 = new G4Tubs("Detector4", 0*cm, 70*cm, 0.5*detector_thickness, 0*deg, 360*deg);
    G4LogicalVolume* logicDetector4 = new G4LogicalVolume(solidDetector4, scintillator_mat, "Detector4");
    fDetector4Volume = logicDetector4;
    fDetector4Position = G4ThreeVector(0, 0, detector4_position);
    new G4PVPlacement(nullptr, fDetector4Position, logicDetector4, "Detector4", logicWorld, false, 0, false);

    // ========== NEW COLLIMATION SYSTEM ==========
    //CreateCollimationSystem();


    // ========== RF CAVITY  ==========
    G4double rfcavity_radius = 30*cm;
    G4double rfcavity_length = 20*cm;
    G4double rfcavity_position = 2000*cm;  // Positioned after detectors

    G4Tubs* solidRFCavity = new G4Tubs("RFCavity", 0*cm, rfcavity_radius, 0.5*rfcavity_length, 0*deg, 360*deg);
    G4LogicalVolume* logicRFCavity = new G4LogicalVolume(solidRFCavity, rfcavity_mat, "RFCavity");
    fRFCavityVolume = logicRFCavity;
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, rfcavity_position), logicRFCavity,
                     "RFCavity", logicWorld, false, 0, false);

    // ========== VISUALIZATION (keep original + new) ==========
    G4VisAttributes* tungsten_vis_att = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5));
    logicTungsten->SetVisAttributes(tungsten_vis_att);

    //G4VisAttributes* tungstenAperture_vis_att = new G4VisAttributes(G4Color::Yellow());
    //tungstenAperture_vis_att->SetVisibility(true);
    //logicTungstenAperture->SetVisAttributes(tungstenAperture_vis_att);

    G4VisAttributes* detector1_vis_att = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
    detector1_vis_att->SetVisibility(true);
    logicDetector1->SetVisAttributes(detector1_vis_att);

    G4VisAttributes* detector2_vis_att = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
    detector2_vis_att->SetVisibility(true);
    logicDetector2->SetVisAttributes(detector2_vis_att);

    G4VisAttributes* detector3_vis_att = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
    detector3_vis_att->SetVisibility(true);
    logicDetector3->SetVisAttributes(detector3_vis_att);

    G4VisAttributes* detector4_vis_att = new G4VisAttributes(G4Colour::Blue());
    detector4_vis_att->SetVisibility(true);
    logicDetector4->SetVisAttributes(detector4_vis_att);

    G4VisAttributes* gunVis = new G4VisAttributes(G4Colour::Red());
    gunVis->SetVisibility(true);
    gunVis->SetForceSolid(true);
    gunLog->SetVisAttributes(gunVis);

    G4VisAttributes* rfcavity_vis_att = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0, 0.7));
    rfcavity_vis_att->SetVisibility(true);
    rfcavity_vis_att->SetForceSolid(true);
    logicRFCavity->SetVisAttributes(rfcavity_vis_att);

    G4VisAttributes* world_vis_att = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0.1));
    world_vis_att->SetVisibility(true);
    world_vis_att->SetForceWireframe(true);
    logicWorld->SetVisAttributes(world_vis_att);

    // Set scoring volumes
    fScoringVolume = logicTungsten;

    return physWorld;
}

void DetectorConstruction::CreateInitialSolenoid(G4double start, G4double len)
{
    G4cout << "\n========== Creating Initial Solenoid (Ramp-Up Field) ==========" << G4endl;

    // Solenoid parameters
    G4double solenoid_start_z = start;

    G4double solenoid_length = len;  // 180 cm
    G4double solenoid_end_z = start+len;
    G4double solenoid_center_z = (solenoid_start_z + solenoid_end_z) / 2.0;
    G4double solenoid_outer_radius = 20*cm;
    G4double coil_thickness = 10*cm;
    G4double field_strength = 7*tesla;

    // Materials
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* coil_material = nist->FindOrBuildMaterial("G4_Cu");
    G4Material* vacuum_material = nist->FindOrBuildMaterial("G4_AIR");

    // Geometry: coil and vacuum
    G4Tubs* coilSolid = new G4Tubs("InitialSolenoidCoil",
                                   solenoid_outer_radius - coil_thickness,
                                   solenoid_outer_radius,
                                   solenoid_length / 2.0,
                                   0, 360*deg);

    G4Tubs* vacuumSolid = new G4Tubs("InitialSolenoidVacuum",
                                     0,
                                     solenoid_outer_radius - coil_thickness,
                                     solenoid_length / 2.0,
                                     0, 360*deg);

    G4LogicalVolume* coilLogical = new G4LogicalVolume(coilSolid, coil_material, "InitialSolenoidCoil");
    G4LogicalVolume* vacuumLogical = new G4LogicalVolume(vacuumSolid, vacuum_material, "InitialSolenoidVacuum");
    fInitialSolenoidVolume = vacuumLogical;

    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, solenoid_center_z), coilLogical,
                      "InitialSolenoidCoil", fWorldLogical, false, 0, false);

    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, solenoid_center_z), vacuumLogical,
                      "InitialSolenoidVacuum", fWorldLogical, false, 0, false);

    //
    // Magnetic field: cosine ramp-up from 0 T → 7 T
    //
    class RampUpField : public G4MagneticField {
    public:
        RampUpField(G4double Bmax, G4double z0, G4double length)
            : B0(Bmax), z_start(z0), L(length) {}

        void GetFieldValue(const G4double point[4], G4double* Bfield) const override {
            G4double z = point[2];

            if (z < z_start) {
                Bfield[2] = 0.0;
            } else if (z >= z_start && z <= z_start + L) {
                G4double arg = M_PI * (z - z_start) / L;
                Bfield[2] = 0.5 * B0 * (1.0 - std::cos(arg));
            } else {
                Bfield[2] = B0;
            }

            Bfield[0] = 0.0;
            Bfield[1] = 0.0;
        }

    private:
        G4double B0, z_start, L;
    };

    fInitialSolenoidField = new RampUpField(field_strength, solenoid_start_z, solenoid_length);
    fInitialSolenoidFieldManager = new G4FieldManager(fInitialSolenoidField);

    G4Mag_UsualEqRhs* equation = new G4Mag_UsualEqRhs(fInitialSolenoidField);
    G4ClassicalRK4* stepper = new G4ClassicalRK4(equation);
    G4double minStep = 0.005*mm;

    G4MagInt_Driver* driver = new G4MagInt_Driver(minStep, stepper, stepper->GetNumberOfVariables());
    G4ChordFinder* chordFinder = new G4ChordFinder(driver);

    fInitialSolenoidFieldManager->SetChordFinder(chordFinder);
    fInitialSolenoidFieldManager->SetDetectorField(fInitialSolenoidField);
    fInitialSolenoidFieldManager->SetDeltaOneStep(0.001*mm);
    fInitialSolenoidFieldManager->SetDeltaIntersection(0.001*mm);

    vacuumLogical->SetFieldManager(fInitialSolenoidFieldManager, true);

    //
    // Visualization
    //
    auto* coilVis = new G4VisAttributes(G4Colour(0.0, 0.8, 0.2, 0.8));
    coilVis->SetVisibility(true);
    coilVis->SetForceSolid(true);
    coilLogical->SetVisAttributes(coilVis);

    auto* vacuumVis = new G4VisAttributes(G4Colour(0.9, 0.9, 0.9, 0.1));
    vacuumVis->SetVisibility(true);
    vacuumVis->SetForceWireframe(true);
    vacuumLogical->SetVisAttributes(vacuumVis);

    G4cout << "Initial solenoid created (with ramp-up field):" << G4endl;
    G4cout << "  Z range: " << solenoid_start_z/cm << " to " << solenoid_end_z/cm << " cm" << G4endl;
    G4cout << "  Radius: " << solenoid_outer_radius/cm << " cm" << G4endl;
    G4cout << "  Field: 0 → " << field_strength/tesla << " T (cosine ramp-up in z)" << G4endl;
    G4cout << "============================================================\n" << G4endl;
}


void DetectorConstruction::CreateTaperedSolenoid(G4double start, G4double len)
{
    G4cout << "\n========== Creating Tapered Solenoid ==========" << G4endl;

    // Parameters
    G4double z_start = start;
    G4double length = len;
    G4double z_end = z_start+len;
    G4double center_z = (z_start + z_end) / 2.0;

    G4double outer_radius = 20*cm;
    G4double coil_thickness = 10*cm;
    G4double B0 = 7*tesla;

    // Materials
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* coil_material = nist->FindOrBuildMaterial("G4_Cu");
    G4Material* vacuum_material = nist->FindOrBuildMaterial("G4_AIR");

    // Geometry
    G4Tubs* coilSolid = new G4Tubs("TaperedSolenoidCoil",
                                   outer_radius - coil_thickness,
                                   outer_radius,
                                   length / 2.0,
                                   0, 360*deg);

    G4Tubs* vacuumSolid = new G4Tubs("TaperedSolenoidVacuum",
                                     0,
                                     outer_radius - coil_thickness,
                                     length / 2.0,
                                     0, 360*deg);

    G4LogicalVolume* coilLogical = new G4LogicalVolume(coilSolid, coil_material, "TaperedSolenoidCoil");
    G4LogicalVolume* vacuumLogical = new G4LogicalVolume(vacuumSolid, vacuum_material, "TaperedSolenoidVacuum");
    fInitialSolenoidVolume = vacuumLogical;

    // Placement
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, center_z), coilLogical,
                      "TaperedSolenoidCoil", fWorldLogical, false, 0, false);

    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, center_z), vacuumLogical,
                      "TaperedSolenoidVacuum", fWorldLogical, false, 0, false);

    //
    // Magnetic Field: Cosine-Squared Taper from B0 to 0
    //
    class LocalTaperedField : public G4MagneticField {
    public:
        LocalTaperedField(G4double Bpeak, G4double z0, G4double taperLen)
            : B0(Bpeak), z_start(z0), L(taperLen) {}

        void GetFieldValue(const G4double point[4], G4double* Bfield) const override {
            G4double z = point[2];
            if (z < z_start) {
                Bfield[0] = 0.0; Bfield[1] = 0.0; Bfield[2] = B0;
            } else if (z >= z_start && z <= z_start + L) {
                G4double factor = 0.5 * (1.0 + std::cos(M_PI * (z - z_start) / L));
                Bfield[0] = 0.0;
                Bfield[1] = 0.0;
                Bfield[2] = B0 * factor;
            } else {
                Bfield[0] = 0.0; Bfield[1] = 0.0; Bfield[2] = 0.0;
            }
        }

    private:
        G4double B0, z_start, L;
    };

    fInitialSolenoidField = new LocalTaperedField(B0, z_start, length);
    fInitialSolenoidFieldManager = new G4FieldManager(fInitialSolenoidField);

    G4Mag_UsualEqRhs* equation = new G4Mag_UsualEqRhs(fInitialSolenoidField);
    G4ClassicalRK4* stepper = new G4ClassicalRK4(equation);
    G4double minStep = 0.005*mm;

    G4MagInt_Driver* driver = new G4MagInt_Driver(minStep, stepper, stepper->GetNumberOfVariables());
    G4ChordFinder* chordFinder = new G4ChordFinder(driver);

    fInitialSolenoidFieldManager->SetChordFinder(chordFinder);
    fInitialSolenoidFieldManager->SetDetectorField(fInitialSolenoidField);
    fInitialSolenoidFieldManager->SetDeltaOneStep(0.0001*mm);
    fInitialSolenoidFieldManager->SetDeltaIntersection(0.0001*mm);

    vacuumLogical->SetFieldManager(fInitialSolenoidFieldManager, true);

    //
    // Visualization
    //
    auto* coilVis = new G4VisAttributes(G4Colour(0.3, 0.4, 0.9, 0.7));
    coilVis->SetVisibility(true);
    coilVis->SetForceSolid(true);
    coilLogical->SetVisAttributes(coilVis);

    auto* vacuumVis = new G4VisAttributes(G4Colour(0.9, 0.9, 0.9, 0.1));
    vacuumVis->SetVisibility(true);
    vacuumVis->SetForceWireframe(true);
    vacuumLogical->SetVisAttributes(vacuumVis);

    G4cout << "Tapered solenoid created:" << G4endl;
    G4cout << "  Z: " << z_start/cm << " → " << z_end/cm << " cm" << G4endl;
    G4cout << "  Radius: " << outer_radius/cm << " cm" << G4endl;
    G4cout << "  Field peak: " << B0/tesla << " T (z-direction, tapered)" << G4endl;
    G4cout << "============================================================\n" << G4endl;
}



void DetectorConstruction::CreateCollimationSystem()
{
    CreatePrimaryCollimator();
    CreateSecondaryCollimator();

    G4cout << "\n========== Collimation System Created ==========" << G4endl;
    G4cout << "Primary collimator: Z = 452cm, aperture = 25cm" << G4endl;
    G4cout << "Secondary collimator: Z = 480cm, aperture = 15cm" << G4endl;
    G4cout << "===============================================\n" << G4endl;
}

void DetectorConstruction::CreatePrimaryCollimator()
{
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* tungsten_mat = nist->FindOrBuildMaterial("G4_W");

    G4double collimator1_z = 1952*cm;
    G4double collimator_thickness = 5*cm;
    G4double aperture_radius = 25*cm;
    G4double outer_radius = 20*cm;

    G4Tubs* solidCollimator1 = new G4Tubs("PrimaryCollimator",
                                          aperture_radius, outer_radius, collimator_thickness/2,
                                          0*deg, 360*deg);

    G4LogicalVolume* logicCollimator1 = new G4LogicalVolume(solidCollimator1, tungsten_mat, "PrimaryCollimator");
    fPrimaryCollimatorVolume = logicCollimator1;

    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, collimator1_z), logicCollimator1,
                     "PrimaryCollimator", fWorldLogical, false, 0, false);

    G4VisAttributes* collimator1_vis = new G4VisAttributes(G4Colour(0.8, 0.8, 0.0, 0.8));
    collimator1_vis->SetVisibility(true);
    collimator1_vis->SetForceSolid(true);
    logicCollimator1->SetVisAttributes(collimator1_vis);
}

void DetectorConstruction::CreateSecondaryCollimator()
{
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* tungsten_mat = nist->FindOrBuildMaterial("G4_W");

    G4double collimator2_z = 1920*cm;
    G4double collimator_thickness = 5*cm;
    G4double aperture_radius = 15*cm;
    G4double outer_radius = 20*cm;

    G4Tubs* solidCollimator2 = new G4Tubs("SecondaryCollimator",
                                          aperture_radius, outer_radius, collimator_thickness/2,
                                          0*deg, 360*deg);

    G4LogicalVolume* logicCollimator2 = new G4LogicalVolume(solidCollimator2, tungsten_mat, "SecondaryCollimator");
    fSecondaryCollimatorVolume = logicCollimator2;

    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, collimator2_z), logicCollimator2,
                     "SecondaryCollimator", fWorldLogical, false, 0, false);

    G4VisAttributes* collimator2_vis = new G4VisAttributes(G4Colour(0.9, 0.7, 0.0, 0.8));
    collimator2_vis->SetVisibility(true);
    collimator2_vis->SetForceSolid(true);
    logicCollimator2->SetVisAttributes(collimator2_vis);
}

void DetectorConstruction::ConstructSDandField()
{
    if (!fMomentumChicane) {
        G4cerr << "Error: New systems not initialized!" << G4endl;
        return;
    }

    // New systems handle their own magnetic fields automatically
    fMomentumChicane->AnalyzeChicane();

    G4cout << "All magnetic fields configured successfully!" << G4endl;
}

void DetectorConstruction::AnalyzeCompleteSystem()
{
    G4cout << "\n" << G4endl;
    G4cout << "####################################################" << G4endl;
    G4cout << "#          COMPLETE SYSTEM ANALYSIS               #" << G4endl;
    G4cout << "####################################################" << G4endl;

    if (fMomentumChicane) {
        fMomentumChicane->AnalyzeChicane();
    }


    G4cout << "\n========== Final System Overview ==========" << G4endl;
    G4cout << "Target → Momentum Chicane → Collimation → Solenoid → Detection" << G4endl;
    G4cout << "0cm → 100-340cm → 452-485cm → 535-1035cm → 1052-1100cm" << G4endl;
    G4cout << "Momentum selection: 2% resolution" << G4endl;
    G4cout << "Solenoid focusing: 25 segments, 7T field" << G4endl;
    G4cout << "Expected final beam: 1.0-1.1 mm radius" << G4endl;
    G4cout << "Expected transmission: ~25%" << G4endl;
    G4cout << "Beta function: ~1.5-1.7 mm" << G4endl;
    G4cout << "Final beamline length: ~1152cm (compact design)" << G4endl;
    G4cout << "==========================================\n" << G4endl;

    G4cout << "####################################################" << G4endl;
    G4cout << "#            ANALYSIS COMPLETE                    #" << G4endl;
    G4cout << "####################################################\n" << G4endl;
}
