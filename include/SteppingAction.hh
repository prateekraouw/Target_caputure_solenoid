#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include <map>
#include <fstream>
#include <vector>
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4FieldManager.hh"
#include "G4MagneticField.hh"
#include "G4TransportationManager.hh"

class EventAction;
class G4LogicalVolume;

// Struct to store particle information
struct ParticleInfo {
  G4String type;
  G4double energy;
  G4ThreeVector position;
  G4ThreeVector direction;
  G4int detectorID;
  G4String process;
};

class SteppingAction : public G4UserSteppingAction
{
public:
  SteppingAction(EventAction* eventAction);
  virtual ~SteppingAction();
  
  // Method called for each step
  virtual void UserSteppingAction(const G4Step*);

  void EnableFieldLogging(bool enable = true) { fLogField = enable; }
  void SetLogFileName(const G4String& filename) { fLogFileName = filename; }
  void SetLogInterval(G4int interval) { fLogInterval = interval; }
  
private:
  EventAction* fEventAction;
  G4LogicalVolume* fScoringVolume;
  G4LogicalVolume* fDetector1Volume;
  G4LogicalVolume* fDetector2Volume;
  G4LogicalVolume* fDetector3Volume;
  G4LogicalVolume* fDetector4Volume;
  G4LogicalVolume* fRFCavityVolume; // Add RF cavity volume

  
  G4LogicalVolume* fCounterVolume;  // Add this for the 10m detector
  
  // Maps to count particles at each detector
  std::map<G4String, G4int> fParticleCounter;
  std::map<G4String, G4int> fDetector1Particles;
  std::map<G4String, G4int> fDetector2Particles;
  std::map<G4String, G4int> fDetector3Particles;
  std::map<G4String, G4int> fDetector4Particles;
  std::map<G4String, G4int> fRFCavityParticles;
    
  // Maps to store entry properties for RF cavity energy gain calculation
  std::map<G4int, G4double> fRFCavityEntranceEnergy;
  std::map<G4int, G4ThreeVector> fRFCavityEntranceMomentum;

  // NEW: Magnetic field logging members
  bool fLogField;
  G4String fLogFileName;
  std::ofstream fLogFile;
  G4int fFieldStepCounter;
  G4int fLogInterval;
  bool fFileInitialized;
    
    // NEW: Helper methods for field logging
  void InitializeFieldLogFile();
  void LogFieldAtPosition(const G4ThreeVector& position, G4int trackID, G4int stepNumber, G4Track* track);
  G4ThreeVector GetMagneticFieldAtPosition(const G4ThreeVector& position, G4Track* track);


  std::map<G4String, G4int> fPrimaryCollimatorLosses;
  std::map<G4String, G4int> fSecondaryCollimatorLosses;

  std::map<G4String, G4int> fCounter10mParticles;
  
  // Store detected particle information
  std::vector<ParticleInfo> fDetectedParticles;
};

#endif