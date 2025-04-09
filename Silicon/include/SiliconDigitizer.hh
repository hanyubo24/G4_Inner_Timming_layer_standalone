#ifndef SiliconDigitizer_h
#define SiliconDigitizer_h 1

#include "G4VDigitizerModule.hh"
#include "G4SystemOfUnits.hh"
#include "SiliconDigiCollection.hh"

class SiliconDigitizer : public G4VDigitizerModule {
public:
  explicit SiliconDigitizer(const G4String& name);
  ~SiliconDigitizer() override;

  void Digitize() override;

private:
  G4double ePairEnergy = 3.6 * eV;   // e-h pair creation energy
  G4double driftVelocity = 1.0e6 * cm/s; 
  G4double diffusionCons = 35. *cm2/s;
  G4double timeSmearing = 30 * ps;  // electronics resolution
  G4double gain = 10.0;

  G4double Mass_e = 0.511 *MeV;
  G4double Mass_mu = 105.66 *MeV;
  G4double Mass_pi = 139.57 *MeV;
  G4double Mass_kaon = 493.68 *MeV;
  G4double Mass_p = 938.27 *MeV;

  const G4double c_light = 299.792458 * mm/ns;
  G4int hitColID = -1; // Cache hit collection ID
  std::vector<double> GenerateWaveform(double edep, double time);
};

#endif


