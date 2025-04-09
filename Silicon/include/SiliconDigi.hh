#ifndef SiliconDigi_h
#define SiliconDigi_h 1

#include "G4VDigi.hh"
#include "G4ThreeVector.hh"

class SiliconDigi : public G4VDigi {
public:
  SiliconDigi();
  ~SiliconDigi() override = default;

  void SetCharge(G4double c) { fCharge = c; }
  void SetTime(G4double t) { fTime = t; }
  void SetWaveform(const std::vector<double>& wf) { fWaveform = wf; }

  G4double GetCharge() const { return fCharge; }
  G4double GetTime() const { return fTime; }
  const std::vector<double>& GetWaveform() const { return fWaveform; }

  void Print() override;

private:
  G4double fCharge; // in electrons
  G4double fTime;   // in ns
  std::vector<double> fWaveform; // waveform
};

#endif
