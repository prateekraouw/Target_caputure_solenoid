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
thread_local std::ofstream RunAction::fParticleFile;
thread_local std::ofstream RunAction::f6DVectorFile;

RunAction::RunAction()
: G4UserRunAction()
{
    G4cout << "[RunAction::RunAction] Constructed" << G4endl;
    // No accumulable registration needed
}

RunAction::~RunAction()
{
    G4cout << "[RunAction::~RunAction] Destructor called" << G4endl;
  // Do not close files here; all files are closed in EndOfRunAction
}

void RunAction::BeginOfRunAction(const G4Run* run)
{
  G4cout << "[RunAction::BeginOfRunAction] Run " << run->GetRunID() << " start." << G4endl;
  fSecondaryParticles.clear();
  fParticleCounts.clear();
  
  // Clear thread-local data at the beginning of each run
  fParticleData.clear();
  f6DVectorData.clear();
  
  // Open per-thread particle data file
  G4int threadID = G4Threading::G4GetThreadId();
  G4int runID = run->GetRunID();
  std::ostringstream filename;
  filename << "particle_data_run" << runID << "_thread" << threadID << ".csv";
  fParticleFile.open(filename.str());
  if (fParticleFile.is_open()) {
    fParticleFile << "ParticleType,Energy[MeV]" << std::endl;
    G4cout << "[RunAction::BeginOfRunAction] Thread " << threadID << " opened particle file: " << filename.str() << G4endl;
  } else {
    G4cerr << "[RunAction::BeginOfRunAction] ERROR: Could not open particle file: " << filename.str() << G4endl;
  }

  // Open per-thread 6D vector file
  std::ostringstream filename6D;
  filename6D << "6D_vector_thread" << threadID << ".csv";
  f6DVectorFile.open(filename6D.str());
  if (f6DVectorFile.is_open()) {
    f6DVectorFile << "Detector,ParticleType,x[cm],px[MeV/c],y[cm],py[MeV/c],z[cm],pz[MeV/c],TotalEnergy[MeV]" << std::endl;
    G4cout << "[RunAction::BeginOfRunAction] Thread " << threadID << " opened 6D vector file: " << filename6D.str() << G4endl;
  } else {
    G4cerr << "[RunAction::BeginOfRunAction] ERROR: Could not open 6D vector file: " << filename6D.str() << G4endl;
  }
}

void RunAction::EndOfRunAction(const G4Run* run)
{
    G4int threadID = G4Threading::G4GetThreadId();
    G4int runID = run->GetRunID();
    G4cout << "[RunAction::EndOfRunAction] Called for run " << runID << ", thread " << threadID << G4endl;

    // Write per-thread particle data
    if (fParticleFile.is_open()) {
        for (const auto& entry : fParticleData) {
            fParticleFile << entry.first << "," << entry.second/MeV << std::endl;
        }
        fParticleFile.close();
        G4cout << "[RunAction::EndOfRunAction] Thread " << threadID << " wrote " << fParticleData.size()
               << " particle entries and closed file." << G4endl;
    } else {
        G4cerr << "[RunAction::EndOfRunAction] ERROR: Particle file was not open for thread " << threadID << G4endl;
    }

    // Write per-thread 6D vector data
    if (f6DVectorFile.is_open()) {
        for (const auto& entry : f6DVectorData) {
            f6DVectorFile << std::get<0>(entry) << ","
                  << std::get<1>(entry) << ","
                  << std::get<2>(entry).x()/cm << "," << std::get<3>(entry).x()/MeV << ","
                  << std::get<2>(entry).y()/cm << "," << std::get<3>(entry).y()/MeV << ","
                  << std::get<2>(entry).z()/cm << "," << std::get<3>(entry).z()/MeV << ","
                  << std::get<4>(entry)/MeV << std::endl;
        }
        f6DVectorFile.close();
        G4cout << "[RunAction::EndOfRunAction] Thread " << threadID << " wrote " << f6DVectorData.size()
               << " 6D vector entries and closed file." << G4endl;
    } else {
        G4cerr << "[RunAction::EndOfRunAction] ERROR: 6D vector file was not open for thread " << threadID << G4endl;
    }
}

void RunAction::RecordParticleToExcel(const G4String& name, const G4double& kineticEnergy)
{
  fParticleData.emplace_back(name, kineticEnergy);
  CountParticle(name);
  G4cout << "[RunAction::RecordParticleToExcel] Recorded " << name << ", " << kineticEnergy/MeV << " MeV" << G4endl;
}

void RunAction::Record6DVector(G4int detectorID, const G4String& particleName, const G4ThreeVector& position, const G4ThreeVector& momentum, G4double totalEnergy)
{
  f6DVectorData.emplace_back(detectorID, particleName, position, momentum, totalEnergy);
  G4cout << "[RunAction::Record6DVector] 6D Vector: Detector " << detectorID 
         << ", Particle " << particleName 
         << ", Energy " << totalEnergy/MeV << " MeV" << G4endl;
}

// Function to save the magnetic field data along the Z-axis to a CSV file
void RunAction::SaveMagneticFieldAlongZ()
{
    G4cout << "[RunAction::SaveMagneticFieldAlongZ] Called" << G4endl;
    // Implementation for magnetic field data saving
}

// Custom exception handler to suppress specific warnings
void RunAction::SuppressWarnings()
{
    G4cout << "[RunAction::SuppressWarnings] Called (no-op)" << G4endl;
}

/*int main(int argc, char** argv) {
    G4MTRunManager* runManager = new G4MTRunManager;
    runManager->SetNumberOfThreads(10); // Replace N with the number of threads you want
    // ... rest of your setup ...
    runManager->BeamOn(1000);
    delete runManager;
    return 0;
}*/