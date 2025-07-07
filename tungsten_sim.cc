#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "Randomize.hh"
#include "G4MTRunManager.hh"
#include "G4Threading.hh"




int main(int argc, char** argv){
  // Remove this line to actually run the simulation
  // if(true)return 0;
  
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  
  // Construct the default run manager
  auto* runManager = G4RunManagerFactory::CreateRunManager();
#ifdef G4MULTITHREADED
    // Set number of threads (default: 10)
    static_cast<G4MTRunManager*>(runManager)->SetNumberOfThreads(10);
    G4cout << "Geant4 running with "
           << static_cast<G4MTRunManager*>(runManager)->GetNumberOfThreads()
           << " threads." << G4endl;
#endif
  
  // Set mandatory initialization classes
  // Use DetectorConstruction instead of ChicaneConstruction
  if (argc >= 4) {
      double gap1 = std::stod(argv[2]);
      double gap2 = std::stod(argv[3]);
      runManager->SetUserInitialization(new DetectorConstruction(gap1, gap2));
  } else {
      runManager->SetUserInitialization(new DetectorConstruction());
  }
  runManager->SetUserInitialization(new PhysicsList());
  runManager->SetUserInitialization(new ActionInitialization());
  
  // Initialize G4 kernel
  runManager->Initialize();
  
  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive();
  visManager->Initialize();
  
  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  
  if (argc != 1) {
    // Batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  }
  else {
    // Interactive mode
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }
  
  // Job termination
  delete visManager;
  delete runManager;
  
  return 0;
}