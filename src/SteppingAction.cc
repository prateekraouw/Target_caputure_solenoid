#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"
#include "RunAction.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4FieldManager.hh"
#include "G4MagneticField.hh"
#include "G4TransportationManager.hh"
#include <iomanip>
#include <memory>

SteppingAction::SteppingAction(EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fScoringVolume(nullptr),
  fDetector1Volume(nullptr),
  fDetector2Volume(nullptr),
  fDetector3Volume(nullptr),
  fDetector4Volume(nullptr),
  fRFCavityVolume(nullptr)
{}

SteppingAction::~SteppingAction()
{
  // Your existing destructor code unchanged
  // Print out the particle count at the end, filtering for muons and pions only
  G4cout << "=== Muons and Pions Generated ===" << G4endl;
  for (auto const& pair : fParticleCounter) {
    // Only include muons and pions in the summary
    if (pair.first == "mu+" || pair.first == "mu-" ||
        pair.first == "pi+" || pair.first == "pi-") {
      G4cout << pair.first << ": " << pair.second << G4endl;
    }
  }
  G4cout << "===================================" << G4endl;

  // Add a section for particles detected at detector 1
  G4cout << "\n=== Muons and Pions Detected at Detector 1 ===" << G4endl;
  for (auto const& pair : fDetector1Particles) {
    G4cout << pair.first << ": " << pair.second << G4endl;
  }
  G4cout << "==========================================" << G4endl;

  // Add a section for particles detected at detector 2
  G4cout << "\n=== Muons and Pions Detected at Detector 2 ===" << G4endl;
  for (auto const& pair : fDetector2Particles) {
    G4cout << pair.first << ": " << pair.second << G4endl;
  }
  G4cout << "==========================================" << G4endl;

  // Add a section for particles detected at detector 3
  G4cout << "\n=== Muons and Pions Detected at Detector 3 ===" << G4endl;
  for (auto const& pair : fDetector3Particles) {
    G4cout << pair.first << ": " << pair.second << G4endl;
  }
  G4cout << "==========================================" << G4endl;

  // Add a section for particles through RF cavity
  G4cout << "\n=== Muons and Pions Through RF Cavity ===" << G4endl;
  for (auto const& pair : fRFCavityParticles) {
    G4cout << pair.first << ": " << pair.second << G4endl;
  }
  G4cout << "==========================================" << G4endl;

  // Add a section for particles detected at detector 4
  G4cout << "\n=== Muons and Pions Detected at Detector 4 ===" << G4endl;
  for (auto const& pair : fDetector4Particles) {
    G4cout << pair.first << ": " << pair.second << G4endl;
  }
  G4cout << "==========================================" << G4endl;
}


void SteppingAction::UserSteppingAction(const G4Step* step)
{
  // ========================================================================
  // ALL YOUR EXISTING CODE BELOW REMAINS COMPLETELY UNCHANGED
  // ========================================================================

  // Initialize volumes if not already done
  if (!fScoringVolume || !fDetector1Volume || !fDetector2Volume || !fDetector3Volume || !fDetector4Volume || !fRFCavityVolume) {
    const DetectorConstruction* detectorConstruction
      = static_cast<const DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume = detectorConstruction->GetScoringVolume();
    fDetector1Volume = detectorConstruction->GetDetector1Volume();
    fDetector2Volume = detectorConstruction->GetDetector2Volume();
    fDetector3Volume = detectorConstruction->GetDetector3Volume();
    fDetector4Volume = detectorConstruction->GetDetector4Volume();
    fRFCavityVolume = detectorConstruction->GetRFCavityVolume();

    G4cout << "Detector 1 position: " << detectorConstruction->GetDetector1Position()/cm << " cm" << G4endl;
    G4cout << "Detector 2 position: " << detectorConstruction->GetDetector2Position()/cm << " cm" << G4endl;
    G4cout << "Detector 3 position: " << detectorConstruction->GetDetector3Position()/cm << " cm" << G4endl;
    G4cout << "Detector 4 position: " << detectorConstruction->GetDetector4Position()/cm << " cm" << G4endl;
  }

  // Get the RunAction - using const_cast to handle the constness issue
  const G4UserRunAction* constRunAction = G4RunManager::GetRunManager()->GetUserRunAction();
  RunAction* runAction = const_cast<RunAction*>(dynamic_cast<const RunAction*>(constRunAction));

  // Get current track
  G4Track* track = step->GetTrack();
  G4ParticleDefinition* particle = track->GetDefinition();
  G4String particleName = particle->GetParticleName();
  G4double energy = track->GetKineticEnergy();


  // Check for pion decay specifically
  G4String processName = "Unknown";
  const G4VProcess* process = step->GetPostStepPoint()->GetProcessDefinedStep();
  if (process) processName = process->GetProcessName();

  // If this is a decay process
  if (processName == "Decay") {
    // If the current particle is a pion
    if (particleName == "pi+" || particleName == "pi-") {
      // Get secondaries created in this step
      const std::vector<const G4Track*>* secondaries = step->GetSecondaryInCurrentStep();

      if (secondaries && secondaries->size() > 0) {
        for (const G4Track* secTrack : *secondaries) {
          G4String secName = secTrack->GetDefinition()->GetParticleName();

          // If a muon is created from pion decay
          if (secName == "mu+" || secName == "mu-") {
            G4ThreeVector position = step->GetPostStepPoint()->GetPosition();
            G4double secEnergy = secTrack->GetKineticEnergy();

            G4cout << "\n!!! PION DECAY DETECTED !!!" << G4endl;
            G4cout << particleName << " → " << secName << G4endl;
            G4cout << "Position: " << position/mm << " mm" << G4endl;
            G4cout << "Parent Energy: " << energy/MeV << " MeV" << G4endl;
            G4cout << "Muon Energy: " << secEnergy/MeV << " MeV" << G4endl;
          }
        }
      }
    }
  }

  // Get current volume
  G4LogicalVolume* volume
    = step->GetPreStepPoint()->GetTouchableHandle()
      ->GetVolume()->GetLogicalVolume();
  G4String volumeName = volume->GetName();



  // Check for muons and charged pions in detector 1
  if (step->IsFirstStepInVolume() && volume == fDetector1Volume) {
    // Get position and momentum for 6D vector
    G4ThreeVector position = step->GetPreStepPoint()->GetPosition();
    G4ThreeVector momentum = step->GetPreStepPoint()->GetMomentum();
    G4double totalEnergy = step->GetPreStepPoint()->GetTotalEnergy();

    if (particleName == "mu+" || particleName == "mu-") {
      // Count muons
      fDetector1Particles[particleName]++;
      runAction->RecordParticleToExcel(particleName, energy);
      // Add 6D vector recording
      runAction->Record6DVector(1, particleName, position, momentum, totalEnergy);
      // Add to event counts
      if (fEventAction) {
        fEventAction->AddMuonAtDetector1();
      }

      G4cout << "\n!!! MUON DETECTED IN DETECTOR 1 !!!" << G4endl;
      G4cout << "Type: " << particleName << G4endl;
      G4cout << "Energy: " << track->GetKineticEnergy()/MeV << " MeV" << G4endl;
    }
    // Only count charged pions (pi+, pi-)
    else if (particleName == "pi+" || particleName == "pi-") {
      // Count charged pions
      fDetector1Particles[particleName]++;
      runAction->RecordParticleToExcel(particleName, energy);
      // Add 6D vector recording
      runAction->Record6DVector(1, particleName, position, momentum, totalEnergy);
      // Add to event counts
      if (fEventAction) {
        fEventAction->AddPionAtDetector1();
      }

      G4cout << "\n!!! PION DETECTED IN DETECTOR 1 !!!" << G4endl;
      G4cout << "Type: " << particleName << G4endl;
      G4cout << "Energy: " << track->GetKineticEnergy()/MeV << " MeV" << G4endl;
    }
  }

  // Check for muons and charged pions in detector 2
  if (step->IsFirstStepInVolume() && volume == fDetector2Volume) {
    // Get position and momentum for 6D vector
    G4ThreeVector position = step->GetPreStepPoint()->GetPosition();
    G4ThreeVector momentum = step->GetPreStepPoint()->GetMomentum();
    G4double totalEnergy = step->GetPreStepPoint()->GetTotalEnergy();

    if (particleName == "mu+" || particleName == "mu-") {
      // Count muons at Detector 2
      fDetector2Particles[particleName]++;
      G4String recordName = "2" + particleName;
      runAction->RecordParticleToExcel(recordName, energy);
      // Add 6D vector recording
      runAction->Record6DVector(2, particleName, position, momentum, totalEnergy);
      // Add to event counts
      if (fEventAction) {
        fEventAction->AddMuonAtDetector2();
      }

      G4cout << "\n!!! MUON DETECTED IN DETECTOR 2 !!!" << G4endl;
      G4cout << "Type: " << particleName << G4endl;
      G4cout << "Energy: " << track->GetKineticEnergy()/MeV << " MeV" << G4endl;
    }
    // Only count charged pions (pi+, pi-)
    else if (particleName == "pi+" || particleName == "pi-") {
      // Count charged pions at Detector 2
      fDetector2Particles[particleName]++;
      G4String recordName = "2" + particleName;
      runAction->RecordParticleToExcel(recordName, energy);
      // Add 6D vector recording
      runAction->Record6DVector(2, particleName, position, momentum, totalEnergy);
      // Add to event counts
      if (fEventAction) {
        fEventAction->AddPionAtDetector2();
      }

      G4cout << "\n!!! PION DETECTED IN DETECTOR 2 !!!" << G4endl;
      G4cout << "Type: " << particleName << G4endl;
      G4cout << "Energy: " << track->GetKineticEnergy()/MeV << " MeV" << G4endl;
    }
  }

  // Check for muons and charged pions in detector 3
  if (step->IsFirstStepInVolume() && volume == fDetector3Volume) {
    // Get position and momentum for 6D vector
    G4ThreeVector position = step->GetPreStepPoint()->GetPosition();
    G4ThreeVector momentum = step->GetPreStepPoint()->GetMomentum();
    G4double totalEnergy = step->GetPreStepPoint()->GetTotalEnergy();

    if (particleName == "mu+" || particleName == "mu-") {
      // Count muons at Detector 3
      fDetector3Particles[particleName]++;
      G4String recordName = "3" + particleName;
      runAction->RecordParticleToExcel(recordName, energy);
      // Add 6D vector recording
      runAction->Record6DVector(3, particleName, position, momentum, totalEnergy);
      // Add to event counts
      if (fEventAction) {
        fEventAction->AddMuonAtDetector3();
      }

      G4cout << "\n!!! MUON DETECTED IN DETECTOR 3 !!!" << G4endl;
      G4cout << "Type: " << particleName << G4endl;
      G4cout << "Energy: " << track->GetKineticEnergy()/MeV << " MeV" << G4endl;
    }
    // Only count charged pions (pi+, pi-)
    else if (particleName == "pi+" || particleName == "pi-") {
      // Count charged pions at Detector 3
      fDetector3Particles[particleName]++;
      G4String recordName = "3" + particleName;
      runAction->RecordParticleToExcel(recordName, energy);
      // Add 6D vector recording
      runAction->Record6DVector(3, particleName, position, momentum, totalEnergy);
      // Add to event counts
      if (fEventAction) {
        fEventAction->AddPionAtDetector3();
      }

      G4cout << "\n!!! PION DETECTED IN DETECTOR 3 !!!" << G4endl;
      G4cout << "Type: " << particleName << G4endl;
      G4cout << "Energy: " << track->GetKineticEnergy()/MeV << " MeV" << G4endl;
    }
  }

  // RF Cavity functionality - track particles entering the cavity
  if (step->IsFirstStepInVolume() && volume == fRFCavityVolume) {
    // Get position and momentum at cavity entrance
    G4ThreeVector position = step->GetPreStepPoint()->GetPosition();
    G4ThreeVector momentum = step->GetPreStepPoint()->GetMomentum();
    G4double totalEnergy = step->GetPreStepPoint()->GetTotalEnergy();

    // Store initial properties for particles of interest
    if (particleName == "mu+" || particleName == "mu-" ||
        particleName == "pi+" || particleName == "pi-") {

      // Count particles
      fRFCavityParticles[particleName]++;

      // Store entry data for later comparison
      fRFCavityEntranceEnergy[track->GetTrackID()] = totalEnergy;
      fRFCavityEntranceMomentum[track->GetTrackID()] = momentum;

      // Record 6D vector for cavity entrance
      G4String recordName = "RF_in_" + particleName;
      runAction->Record6DVector(5, recordName, position, momentum, totalEnergy);

      G4cout << "\n!!! PARTICLE ENTERING RF CAVITY !!!" << G4endl;
      G4cout << "Type: " << particleName << G4endl;
      G4cout << "Energy: " << totalEnergy/MeV << " MeV" << G4endl;
      G4cout << "Momentum: " << momentum.mag()/MeV << " MeV/c" << G4endl;
    }
  }

  // RF Cavity functionality - track particles exiting the cavity
  if (step->IsLastStepInVolume() && volume == fRFCavityVolume) {
    // Get position and momentum at cavity exit
    G4ThreeVector position = step->GetPostStepPoint()->GetPosition();
    G4ThreeVector momentum = step->GetPostStepPoint()->GetMomentum();
    G4double totalEnergy = step->GetPostStepPoint()->GetTotalEnergy();
    G4int trackID = track->GetTrackID();

    // Check if this particle was tracked at entrance
    if (particleName == "mu+" || particleName == "mu-" ||
        particleName == "pi+" || particleName == "pi-") {

      // Record 6D vector for cavity exit
      G4String recordName = "RF_out_" + particleName;
      runAction->Record6DVector(6, recordName, position, momentum, totalEnergy);

      // Calculate energy gain if we tracked this particle at entrance
      if (fRFCavityEntranceEnergy.find(trackID) != fRFCavityEntranceEnergy.end()) {
        G4double initialEnergy = fRFCavityEntranceEnergy[trackID];
        G4ThreeVector initialMomentum = fRFCavityEntranceMomentum[trackID];
        G4double energyGain = totalEnergy - initialEnergy;

        G4cout << "\n!!! PARTICLE EXITING RF CAVITY !!!" << G4endl;
        G4cout << "Type: " << particleName << G4endl;
        G4cout << "Initial Energy: " << initialEnergy/MeV << " MeV" << G4endl;
        G4cout << "Final Energy: " << totalEnergy/MeV << " MeV" << G4endl;
        G4cout << "Energy Gain: " << energyGain/MeV << " MeV" << G4endl;
        G4cout << "Initial Momentum Z: " << initialMomentum.z()/MeV << " MeV/c" << G4endl;
        G4cout << "Final Momentum Z: " << momentum.z()/MeV << " MeV/c" << G4endl;

        // Clean up the tracking maps to avoid memory growth
        fRFCavityEntranceEnergy.erase(trackID);
        fRFCavityEntranceMomentum.erase(trackID);
      }
    }
  }

  // Check for muons and charged pions in detector 4
  if (step->IsFirstStepInVolume() && volume == fDetector4Volume) {
    // Get position and momentum for 6D vector
    G4ThreeVector position = step->GetPreStepPoint()->GetPosition();
    G4ThreeVector momentum = step->GetPreStepPoint()->GetMomentum();
    G4double totalEnergy = step->GetPreStepPoint()->GetTotalEnergy();

    if (particleName == "mu+" || particleName == "mu-") {
      // Count muons at Detector 4
      fDetector4Particles[particleName]++;
      G4String recordName = "4" + particleName;
      runAction->RecordParticleToExcel(recordName, energy);
      // Add 6D vector recording
      runAction->Record6DVector(4, particleName, position, momentum, totalEnergy);
      // Add to event counts
      if (fEventAction) {
        fEventAction->AddMuonAtDetector4();
      }

      G4cout << "\n!!! MUON DETECTED IN DETECTOR 4 !!!" << G4endl;
      G4cout << "Type: " << particleName << G4endl;
      G4cout << "Energy: " << track->GetKineticEnergy()/MeV << " MeV" << G4endl;
    }
    // Only count charged pions (pi+, pi-)
    else if (particleName == "pi+" || particleName == "pi-") {
      // Count charged pions at Detector 4
      fDetector4Particles[particleName]++;
      G4String recordName = "4" + particleName;
      runAction->RecordParticleToExcel(recordName, energy);
      // Add 6D vector recording
      runAction->Record6DVector(4, particleName, position, momentum, totalEnergy);
      // Add to event counts
      if (fEventAction) {
        fEventAction->AddPionAtDetector4();
      }

      G4cout << "\n!!! PION DETECTED IN DETECTOR 4 !!!" << G4endl;
      G4cout << "Type: " << particleName << G4endl;
      G4cout << "Energy: " << track->GetKineticEnergy()/MeV << " MeV" << G4endl;
    }
  }

  // COMPREHENSIVE SOLENOID DETECTION for 23 solenoids
  static std::set<G4String> allSolenoidVolumes;
      static bool discoveryPhase = true;
      static G4int discoverySteps = 0;

      // Discovery phase: find all volumes with magnetic fields
      if (discoveryPhase) {
          discoverySteps++;

          // Check if this volume has a magnetic field
          G4FieldManager* fieldMgr = volume->GetFieldManager();
          if (!fieldMgr) {
              fieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();
          }

          bool hasField = false;
          if (fieldMgr) {
              const G4Field* field = fieldMgr->GetDetectorField();
              if (field) {
                  const G4MagneticField* magField = dynamic_cast<const G4MagneticField*>(field);
                  if (magField) {
                      // Test if field is non-zero at current position
                      G4ThreeVector position = step->GetTrack()->GetPosition();
                      G4double energy = step->GetPreStepPoint()->GetTotalEnergy();
                      G4double point[4] = {position.x(), position.y(), position.z(), 0.0};
                      G4double fieldValue[3];
                      magField->GetFieldValue(point, fieldValue);

                      G4double fieldMagnitude = sqrt(fieldValue[0]*fieldValue[0] +
                                                   fieldValue[1]*fieldValue[1] +
                                                   fieldValue[2]*fieldValue[2]);

                          if (allSolenoidVolumes.find(volumeName) == allSolenoidVolumes.end()) {
                              allSolenoidVolumes.insert(volumeName);
                              G4cout << "SOLENOID " << allSolenoidVolumes.size()
                                     << " DISCOVERED: '" << volumeName
                                     << "' (|B|=" << fieldMagnitude/tesla << "T)" << G4endl;
                          }
                          }
              }
          }

          // End discovery phase after sufficient steps or when we find 23+ solenoids
          if (discoverySteps > 5000 || allSolenoidVolumes.size() >= 23) {
              discoveryPhase = false;
              G4cout << "\n=== DISCOVERY COMPLETE ===" << G4endl;
              G4cout << "Found " << allSolenoidVolumes.size() << " magnetic volumes:" << G4endl;
              G4int counter = 1;
              for (const auto& vol : allSolenoidVolumes) {
                  G4cout << "  " << counter << ". " << vol << G4endl;
                  counter++;
              }
              G4cout << "========================\n" << G4endl;
          }
      }

      // Check if current volume is one of our discovered solenoids
      bool inSolenoid = (allSolenoidVolumes.find(volumeName) != allSolenoidVolumes.end());

      // LOGGING PHASE - Log all 23 solenoids
      static bool csvCreated = false;
      static std::map<G4String, G4int> volumeCounters;
      static G4int totalLoggedPoints = 0;

      if (inSolenoid) {
          // Initialize counter for this volume
          if (volumeCounters.find(volumeName) == volumeCounters.end()) {
              volumeCounters[volumeName] = 0;
          }
          volumeCounters[volumeName]++;

          // Create CSV header once
          if (!csvCreated) {
              std::ofstream csvFile("all_23_solenoids.csv", std::ios::out);
              if (csvFile.is_open()) {
                  csvFile << "x,y,z,bx,by,bz,particle,energy,volume\n";
                  csvFile.close();
                  csvCreated = true;
                  G4cout << "Created all_23_solenoids.csv for logging all "
                         << allSolenoidVolumes.size() << " solenoids" << G4endl;
              }
          }

          // Log every 5th step in each volume independently
          if (volumeCounters[volumeName] % 5 == 0) {
              G4Track* track = step->GetTrack();
              G4String particleName = track->GetDefinition()->GetParticleName();
              G4double energy = step->GetPreStepPoint()->GetTotalEnergy();
              if (particleName == "mu+" || particleName == "mu-" || particleName=="pi+" || particleName == "pi-"){
              // Get field manager
              G4FieldManager* fieldMgr = volume->GetFieldManager();
              if (!fieldMgr) {
                  fieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();
              }

              if (fieldMgr) {
                  const G4Field* field = fieldMgr->GetDetectorField();
                  if (field) {
                      const G4MagneticField* magField = dynamic_cast<const G4MagneticField*>(field);
                      if (magField) {
                          // Get position and field
                          G4ThreeVector position = track->GetPosition();
                          G4double point[4] = {position.x(), position.y(), position.z(), 0.0};
                          G4double fieldValue[3];
                          magField->GetFieldValue(point, fieldValue);

                          // Convert to mm and Tesla
                          G4double x = position.x() / mm;
                          G4double y = position.y() / mm;
                          G4double z = position.z() / mm;
                          G4double bx = fieldValue[0] / tesla;
                          G4double by = fieldValue[1] / tesla;
                          G4double bz = fieldValue[2] / tesla;

                          // Write to CSV
                          std::ofstream csvFile("all_23_solenoids.csv", std::ios::app);
                          if (csvFile.is_open()) {
                              csvFile << std::fixed << std::setprecision(6)
                                     << x << "," << y << "," << z << ","
                                     << bx << "," << by << "," << bz << ","
                                     << particleName<<","<<energy << "," << volumeName << "\n";
                              csvFile.close();
                              totalLoggedPoints++;
                          }
                      }
                  }
              }
          }
        }
      }

      // Periodic status report showing all 23 solenoids
      static G4int lastReportStep = 0;
      static G4int globalStepCounter = 0;
      globalStepCounter++;

      if (globalStepCounter % 1000 == 0 && globalStepCounter != lastReportStep) {
          lastReportStep = globalStepCounter;

          if (!discoveryPhase && allSolenoidVolumes.size() > 0) {
              G4cout << "\n=== STATUS: All " << allSolenoidVolumes.size()
                     << " Solenoids (Step " << globalStepCounter << ") ===" << G4endl;

              G4int activeVolumes = 0;
              for (const auto& vol : allSolenoidVolumes) {
                  if (volumeCounters.find(vol) != volumeCounters.end() && volumeCounters[vol] > 0) {
                      G4cout << "  " << vol << ": " << volumeCounters[vol] << " steps" << G4endl;
                      activeVolumes++;
                  } else {
                      G4cout << "  " << vol << ": not yet visited" << G4endl;
                  }
              }
              G4cout << "Active volumes: " << activeVolumes << "/" << allSolenoidVolumes.size() << G4endl;
              G4cout << "Total logged points: " << totalLoggedPoints << G4endl;
              G4cout << "==========================================\n" << G4endl;
          }
      }


  // Check if we are in scoring volume for energy deposition
  if (volume == fScoringVolume) {
    // Collect energy deposited in this step
    G4double edepStep = step->GetTotalEnergyDeposit();
    fEventAction->AddEdep(edepStep);
  }

}
