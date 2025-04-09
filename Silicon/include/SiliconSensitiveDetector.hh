#ifndef SiliconSensitiveDetector_h
#define SiliconSensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include "SiliconHit.hh"

class SiliconSensitiveDetector : public G4VSensitiveDetector {
public:
  SiliconSensitiveDetector(const G4String& name);
  ~SiliconSensitiveDetector() override = default;

  void Initialize(G4HCofThisEvent* hce) override;
  G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
  void EndOfEvent(G4HCofThisEvent* hce) override;

private:
  SiliconHitsCollection* fHitsCollection = nullptr;
  G4int fHCID = -1;
};

#endif
