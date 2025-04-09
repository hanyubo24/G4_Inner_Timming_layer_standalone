#include "SiliconHit.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"

G4ThreadLocal G4Allocator<SiliconHit>* SiliconHitAllocator = nullptr;

SiliconHit::SiliconHit()
  : fEdep(0.), fPos(G4ThreeVector()), fTime(0.) {}

SiliconHit::SiliconHit(const SiliconHit& right)
  : G4VHit(),
    fEdep(right.fEdep),
    fPos(right.fPos),
    fTime(right.fTime) {}

const SiliconHit& SiliconHit::operator=(const SiliconHit& right) {
  if (this != &right) {
    fEdep = right.fEdep;
    fPos = right.fPos;
    fTime = right.fTime;
  }
  return *this;
}

G4bool SiliconHit::operator==(const SiliconHit& right) const {
  return (this == &right);
}

void* SiliconHit::operator new(size_t) {
  if (!SiliconHitAllocator) SiliconHitAllocator = new G4Allocator<SiliconHit>;
  return (void*) SiliconHitAllocator->MallocSingle();
}

void SiliconHit::operator delete(void* hit) {
  SiliconHitAllocator->FreeSingle((SiliconHit*) hit);
}

void SiliconHit::Print() {
  G4cout << "SiliconHit: E = " << G4BestUnit(fEdep, "Energy")
         << ", Pos = " << G4BestUnit(fPos, "Length")
         << ", Time = " << G4BestUnit(fTime, "Time") << G4endl;
}
