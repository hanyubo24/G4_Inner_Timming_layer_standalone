#include "SiliconDigiCollection.hh"

SiliconDigiCollection::SiliconDigiCollection(const G4String& moduleName,
                                             const G4String& collectionName)
  : G4VDigiCollection(moduleName, collectionName) {}

SiliconDigiCollection::~SiliconDigiCollection() {
  for (auto* d : fDigiList) delete d;
}

void SiliconDigiCollection::insert(SiliconDigi* digi) {
  fDigiList.push_back(digi);
}

SiliconDigi* SiliconDigiCollection::operator[](G4int i) const{
  return fDigiList[i];
}

size_t SiliconDigiCollection::entries() const {
  return fDigiList.size();
}
