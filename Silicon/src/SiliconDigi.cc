#include "SiliconDigi.hh"
#include "G4ios.hh"

SiliconDigi::SiliconDigi() : fCharge(0), fTime(0) {}

void SiliconDigi::Print() {
  G4cout << "Charge = " << fCharge << " e, Time = " << fTime << " ns" << G4endl;
}
