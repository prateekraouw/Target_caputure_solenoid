#ifndef MomentumChicane_h
#define MomentumChicane_h 1

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4MagneticField.hh"
#include "G4FieldManager.hh"
#include "G4SystemOfUnits.hh"
#include <vector>

class UniformMagField;

class MomentumChicane
{
public:
    MomentumChicane();
    ~MomentumChicane();
    
    void ConfigureChicane(G4double zStart, G4double magnetLength, 
                         G4double magnetSeparation, G4double fieldStrength,
                         G4double magnetWidth, G4double magnetHeight);
    
    void BuildChicane(G4LogicalVolume* motherVolume);
    void SetupMagneticFields();
    void AnalyzeChicane();
    
    G4double GetTotalLength() const;
    G4double GetMomentumResolution(G4double momentum) const;
    std::vector<G4LogicalVolume*>& GetMagnetVolumes() { return fMagnetVolumes; }

private:
    G4double fZStart;
    G4double fMagnetLength;
    G4double fMagnetSeparation;
    G4double fFieldStrength;
    G4double fMagnetWidth;
    G4double fMagnetHeight;
    
    std::vector<G4LogicalVolume*> fMagnetVolumes;
    std::vector<G4MagneticField*> fMagneticFields;
    std::vector<G4FieldManager*> fFieldManagers;
    
    G4Material* fMagnetMaterial;
    
    void InitializeMaterials();
    void CreateMagnetGeometry(G4int magnetID, G4LogicalVolume* motherVolume);
    void SetupMagnetField(G4int magnetID);
};

#endif
