#ifndef RFCavityField_h
#define RFCavityField_h 1

#include "G4ElectroMagneticField.hh"
#include "globals.hh"

class RFCavityField : public G4ElectroMagneticField
{
  public:
    RFCavityField();
    virtual ~RFCavityField();
    
    // Method to get the field value at a certain point
    virtual void GetFieldValue(const G4double Point[4], G4double* field) const;
    
    // Set the field parameters
    void SetFieldAmplitude(G4double val) { fAmplitude = val; }
    void SetFieldFrequency(G4double val) { fFrequency = val; }
    void SetFieldPhase(G4double val) { fPhase = val; }
    
  private:
    G4double fAmplitude;  // Field amplitude in V/m
    G4double fFrequency;  // Field frequency in Hz
    G4double fPhase;      // Field phase in radians
};

#endif