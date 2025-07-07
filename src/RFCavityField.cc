#include "RFCavityField.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

RFCavityField::RFCavityField()
: G4ElectroMagneticField(),
  fAmplitude(20.0*megavolt/m),  // Default 20 MV/m electric field strength
  fFrequency(2856*megahertz),   // Default frequency (S-band)
  fPhase(0.0)                   // Default phase
{
}

RFCavityField::~RFCavityField()
{
}

// Electric field is along z-axis (accelerating), oscillating with time
void RFCavityField::GetFieldValue(const G4double Point[4], G4double* field) const
{
  // Time is Point[3]
  G4double time = Point[3];
  
  // Calculate oscillating field: E = E0 * sin(ωt + φ)
  G4double omega = fFrequency * twopi;
  G4double fieldStrength = fAmplitude * std::sin(omega * time + fPhase);
  
  // Set field components
  field[0] = 0.0;              // Ex = 0
  field[1] = 0.0;              // Ey = 0
  field[2] = fieldStrength;    // Ez = accelerating field
  field[3] = 0.0;              // Bx = 0
  field[4] = 0.0;              // By = 0
  field[5] = 0.0;              // Bz = 0
}