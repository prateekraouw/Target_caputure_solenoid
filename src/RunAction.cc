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
  
  // If 6D vector file was closed, reopen it for this run
  if (!file6DVector.is_open()) {
    Open6DVectorFile();
  }
}

void RunAction::EndOfRunAction(const G4Run* run)
{
    // Write per-thread particle data
    std::ostringstream filename;
    filename << "particle_data_thread" << G4Threading::G4GetThreadId() << ".csv";
    std::ofstream out(filename.str());
    out << "ParticleType,Energy" << std::endl;
    for (const auto& entry : fParticleData) {
        out << entry.first << "," << entry.second/MeV << std::endl;
    }
    out.close();

    // Write per-thread 6D vector data
    std::ostringstream filename6D;
    filename6D << "6D_vector_thread" << G4Threading::G4GetThreadId() << ".csv";
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
}

void RunAction::RecordParticleToExcel(const G4String& name, const G4double& kineticEnergy)
{
  fParticleData.emplace_back(name, kineticEnergy);
  CountParticle(name);
}

void RunAction::Record6DVector(G4int detectorID, const G4String& particleName, const G4ThreeVector& position, const G4ThreeVector& momentum, G4double totalEnergy)
{
  f6DVectorData.emplace_back(detectorID, particleName, position, momentum, totalEnergy);
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

 // For file handling

// Function to save the magnetic field data along the Z-axis to a CSV file

/*int main(int argc, char** argv) {
    G4MTRunManager* runManager = new G4MTRunManager;
    runManager->SetNumberOfThreads(10); // Replace N with the number of threads you want
    // ... rest of your setup ...
    runManager->BeamOn(1000);
    delete runManager;
    return 0;
}*/