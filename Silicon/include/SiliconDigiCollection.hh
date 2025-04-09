#ifndef SiliconDigiCollection_h
#define SiliconDigiCollection_h 1

#include "G4VDigiCollection.hh"
#include "globals.hh"
#include "SiliconDigi.hh"
#include <vector>

class SiliconDigiCollection : public G4VDigiCollection {
public:
  SiliconDigiCollection(const G4String& moduleName, const G4String& collectionName);
  ~SiliconDigiCollection() override;

  void insert(SiliconDigi* digi);
  SiliconDigi* operator[](G4int i) const;
  size_t entries() const;

private:
  std::vector<SiliconDigi*> fDigiList;
};

#endif
