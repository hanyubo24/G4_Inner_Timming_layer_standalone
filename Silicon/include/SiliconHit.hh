#ifndef SiliconHit_h
#define SiliconHit_h 1

#include "G4VHit.hh"
#include "G4ThreeVector.hh"
#include "G4Allocator.hh"
#include "G4THitsCollection.hh"

class SiliconHit : public G4VHit {
public:
  SiliconHit();
  ~SiliconHit() override = default;

  SiliconHit(const SiliconHit&);
  const SiliconHit& operator=(const SiliconHit&);

  G4bool operator==(const SiliconHit&) const;

  void* operator new(size_t);
  void  operator delete(void*);

  void SetEdep(G4double edep) { fEdep = edep; }
  void SetTime(G4double time) { fTime = time; }
  void SetPos(const G4ThreeVector& pos) { fPos = pos; }
  void SetStepLength(G4double stepLength) { fstepLength = stepLength; }
  void SetDedx(G4double dedx) { fdedx = dedx; }
  void SetMomIn(G4double mom) { fMomentumIn = mom; }
  void SetParticleMass(G4double mass) { fParticleMass = mass; }
  void SetHitLayer(G4int layer) { fLayer = layer; }
  void SetActualDriftz(G4double ActualDriftz) { fActualDriftz = ActualDriftz; }
  void SetVertex(const G4ThreeVector& vertex) { fVertex = vertex; } 
  void SetPathLength(G4double pathLength) { fpathLength = pathLength; }  


  void AddEdep(G4double edep) { fEdep += edep; }

  G4double GetEdep() const { return fEdep; }
  G4ThreeVector GetPos() const { return fPos; }
  G4double GetTime() const { return fTime; }
  G4double GetStepLength() const { return fstepLength; }
  G4double GetDedx() const { return fdedx; }
  G4double GetMomIn() const { return fMomentumIn; }
  G4double GetParticleMass() const { return fParticleMass; }
  G4double GetHitLayer() const { return fLayer; }
  G4double GetPathLength() const {return fpathLength;}
  G4double GetActualDriftz() const {return fActualDriftz;}
  G4ThreeVector GetVertex() const {return fVertex;}

  void Print() override;

private:
  G4double fEdep;
  G4ThreeVector fPos;
  G4ThreeVector fVertex;
  G4double fpathLength;
  G4double fTime;
  G4double fstepLength;
  G4double fdedx;
  G4double fMomentumIn;
  G4double fParticleMass;
  G4double fActualDriftz;
  G4int fLayer;
};

using SiliconHitsCollection = G4THitsCollection<SiliconHit>;

extern G4ThreadLocal G4Allocator<SiliconHit>* SiliconHitAllocator;

#endif
