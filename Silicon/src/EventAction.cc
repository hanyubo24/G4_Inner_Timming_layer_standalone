//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B4/B4a/src/EventAction.cc
/// \brief Implementation of the B4a::EventAction class

#include "EventAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4DigiManager.hh"
#include "G4UnitsTable.hh"
#include "SiliconDigitizer.hh"
#include "SiliconDigi.hh"
#include <iomanip>

namespace B4a
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* /*event*/)
{
  // initialisation per event
  fEnergyAbs = 0.;
  ChargedHits = 0.;
  TimeHits = 0.;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{

  G4DigiManager* digiManager = G4DigiManager::GetDMpointer();
  digiManager->Digitize("SiliconDigitizer");

  G4int dcID = digiManager->GetDigiCollectionID("SiliconDigitizer/SiliconDigiCollection");
  auto* digits = const_cast<SiliconDigiCollection*>(static_cast<const SiliconDigiCollection*>(digiManager->GetDigiCollection(dcID)));

//  const auto* digits = static_cast<SiliconDigiCollection*>(digiManager->GetDigiCollection(dcID));;

  if (!digits || digits->entries() == 0) return;

  auto* digit = (*digits)[0];
  const std::vector<double>& waveform = digit->GetWaveform();
  auto ChargedHits = digit->GetCharge(); 
  auto TimeHits = digit->GetTime(); 

  // Print per event (modulo n)
  //
  //auto eventID = event->GetEventID();
  //auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  //if ((printModulo > 0) && (eventID % printModulo == 0)) {
  //  G4cout << "   Absorber: total energy: " << std::setw(7) << G4BestUnit(fEnergyAbs, "Energy") << G4endl;
  //  G4cout << "--> End of event " << eventID << "\n" << G4endl;
 // }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

}  // namespace B4a
