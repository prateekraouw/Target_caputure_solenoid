#include "RunAction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4UnitsTable.hh"
#include <fstream> 
#include <vector>
#include <mutex>
#include "G4MTRunManager.hh"
#include "G4Threading.hh"
#include <sstream>

// Define thread-local storage
thread_local std::vector<std::pair<G4String, G4double>> RunAction::fParticleData;
thread_local std::vector<std::tuple<G4int, G4String, G4ThreeVector, G4ThreeVector, G4double>> RunAction::f6DVectorData;

RunAction::RunAction()
: G4UserRunAction()
{
    // No accumulable registration needed
}

RunAction::~RunAction()
{
  if (fOutputFile.is_open()) {
    fOutputFile.close();
  }
  
  // Close the 6D vector file
  Close6DVectorFile();
}

void RunAction::BeginOfRunAction(const G4Run* run)
{
  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;
  fSecondaryParticles.clear();
  fParticleCounts.clear();
  
  // Clear thread-local data at the beginning of each run
  fParticleData.clear();
  f6DVectorData.clear();
  
  // Open Excel file for particle data
  G4String fileName = "particle_data" + std::to_string(run->GetRunID()) + ".csv";
  fOutputFile.open(fileName);
  
  // Write CSV header with more information
  if (fOutputFile.is_open()) {
    fOutputFile << "ParticleType,Energy" << std::endl;
    G4cout << "Recording particle data to file: " << fileName << G4endl;
  } else {
    G4cerr << "ERROR: Could not open output file " << fileName << G4endl;
  }
  
  // Open global 6D vector file only once per run (thread 0)
  G4int threadID = G4Threading::G4GetThreadId();
  if (threadID == 0) {
    // Close any existing file first
    if (file6DVector.is_open()) {
      file6DVector.close();
    }
    Open6DVectorFile();
    G4cout << "Thread 0 opened global 6D vector file for run " << run->GetRunID() << G4endl;
  }
}

void RunAction::EndOfRunAction(const G4Run* run)
{
    // Get current thread ID for unique file naming
    G4int threadID = G4Threading::G4GetThreadId();
    G4int runID = run->GetRunID();
    
    // Write per-thread particle data with thread ID in filename
    std::ostringstream filename;
    filename << "particle_data_run" << runID << "_thread" << threadID << ".csv";
    std::ofstream out(filename.str());
    out << "ParticleType,Energy[MeV]" << std::endl;
    for (const auto& entry : fParticleData) {
        out << entry.first << "," << entry.second/MeV << std::endl;
    }
    out.close();
    
    G4cout << "Thread " << threadID << " wrote " << fParticleData.size() 
           << " particle entries to " << filename.str() << G4endl;

    // Write per-thread 6D vector data with thread ID in filename
    std::ostringstream filename6D;
    filename6D << "6D_vector_run" << runID << "_thread" << threadID << ".csv";
    std::ofstream out6D(filename6D.str());
    out6D << "Detector,ParticleType,x[cm],px[MeV/c],y[cm],py[MeV/c],z[cm],pz[MeV/c],TotalEnergy[MeV]" << std::endl;
    for (const auto& entry : f6DVectorData) {
        out6D << std::get<0>(entry) << ","
              << std::get<1>(entry) << ","
              << std::get<2>(entry).x()/cm << "," << std::get<3>(entry).x()/MeV << ","
              << std::get<2>(entry).y()/cm << "," << std::get<3>(entry).y()/MeV << ","
              << std::get<2>(entry).z()/cm << "," << std::get<3>(entry).z()/MeV << ","
              << std::get<4>(entry)/MeV << std::endl;
    }
    out6D.close();
    
    G4cout << "Thread " << threadID << " wrote " << f6DVectorData.size() 
           << " 6D vector entries to " << filename6D.str() << G4endl;
}

void RunAction::RecordParticleToExcel(const G4String& name, const G4double& kineticEnergy)
{
  // Thread-local storage automatically handles per-thread data
  fParticleData.emplace_back(name, kineticEnergy);
  CountParticle(name);
}

void RunAction::Record6DVector(G4int detectorID, const G4String& particleName, const G4ThreeVector& position, const G4ThreeVector& momentum, G4double totalEnergy)
{
  // Thread-local storage automatically handles per-thread data
  f6DVectorData.emplace_back(detectorID, particleName, position, momentum, totalEnergy);
  
  // Debug output
  G4cout << "6D Vector recorded: Detector " << detectorID 
         << ", Particle " << particleName 
         << ", Energy " << totalEnergy/MeV << " MeV" << G4endl;
  
  // Also write to global 6D vector file immediately with thread safety
  if (file6DVector.is_open()) {
    std::lock_guard<std::mutex> lock(fDataMutex); // Thread safety
    file6DVector << detectorID << ","
                 << particleName << ","
                 << position.x()/cm << "," << momentum.x()/MeV << ","
                 << position.y()/cm << "," << momentum.y()/MeV << ","
                 << position.z()/cm << "," << momentum.z()/MeV << ","
                 << totalEnergy/MeV << std::endl;
    file6DVector.flush(); // Ensure data is written immediately
  } else {
    G4cout << "WARNING: Global 6D vector file is not open!" << G4endl;
  }
}

// Function to open 6D vector file
void RunAction::Open6DVectorFile()
{
  // Create a new file for 6D phase space data
  std::string filename = "6D_vector.csv";
  
  // Open the file in write mode (overwrite if exists)
  file6DVector.open(filename, std::ios::out);
  
  // Add header to the CSV file
  if (file6DVector.is_open()) {
    file6DVector << "Detector,ParticleType,x[cm],px[MeV/c],y[cm],py[MeV/c],z[cm],pz[MeV/c],TotalEnergy[MeV]" << std::endl;
    G4cout << "Opened 6D vector file: " << filename << G4endl;
  } else {
    G4cout << "ERROR: Could not open 6D vector file: " << filename << G4endl;
  }
}

// Function to close 6D vector file
void RunAction::Close6DVectorFile()
{
  if (file6DVector.is_open()) {
    file6DVector.close();
    G4cout << "Closed 6D vector file" << G4endl;
  }
}

// Function to save the magnetic field data along the Z-axis to a CSV file
void RunAction::SaveMagneticFieldAlongZ()
{
    // Implementation for magnetic field data saving
    // This can be implemented as needed
}

// Custom exception handler to suppress specific warnings
void RunAction::SuppressWarnings()
{
    // Get the UI manager to execute commands
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    
    // Suppress geometry navigation warnings
    UImanager->ApplyCommand("/control/exception GeomNav1002 JustWarning");
    UImanager->ApplyCommand("/control/exception GeomNav1001 JustWarning");
    
    // Suppress field integration warnings
    UImanager->ApplyCommand("/control/exception GeomField1001 JustWarning");
    UImanager->ApplyCommand("/control/exception GeomField1002 JustWarning");
    
    // Suppress transportation warnings
    UImanager->ApplyCommand("/control/exception Transportation1001 JustWarning");
    UImanager->ApplyCommand("/control/exception Transportation1002 JustWarning");
    
    // Suppress physics process warnings
    UImanager->ApplyCommand("/control/exception Physics1001 JustWarning");
    UImanager->ApplyCommand("/control/exception Physics1002 JustWarning");
    
    G4cout << "Warning suppression enabled for geometry and field integration" << G4endl;
}

/*int main(int argc, char** argv) {
    G4MTRunManager* runManager = new G4MTRunManager;
    runManager->SetNumberOfThreads(10); // Replace N with the number of threads you want
    // ... rest of your setup ...
    runManager->BeamOn(1000);
    delete runManager;
    return 0;
}*/