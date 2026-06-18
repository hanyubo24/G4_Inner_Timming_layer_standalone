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
#include "G4SDManager.hh"
#include "SiliconHit.hh"
#include "Randomize.hh"
#include <algorithm>
#include <cmath>
#include <map>
#include <tuple>
#include <vector>

namespace
{
G4double SmearedCrossingTime(const SiliconHit* hit, G4double sigmaT)
{
  // Truth boundary-crossing time + Gaussian noise. No drift-time correction
  // (the previous version used a fictitious local-z drift that was physically
  // meaningless for a barrel cylinder and broke when -th was varied).
  return hit->GetTime() + G4RandGauss::shoot(0., sigmaT);
}

void ProcessCurlingHits(SiliconHitsCollection* hitsCol, G4int eventID, G4double sigmaT)
{
  if (!hitsCol || hitsCol->entries() == 0) return;

  using TrackLayerKey = std::tuple<G4int, G4int>;
  std::map<TrackLayerKey, std::vector<SiliconHit*>> grouped;

  for (size_t i = 0; i < hitsCol->entries(); ++i) {
    auto* hit = (*hitsCol)[i];
    if (!hit) continue;
    grouped[{hit->GetTrackID(), hit->GetHitLayer()}].push_back(hit);
  }

  auto* analysisManager = G4AnalysisManager::Instance();

  for (auto& [key, hits] : grouped) {
    std::sort(hits.begin(), hits.end(),
              [](const SiliconHit* a, const SiliconHit* b) { return a->GetTime() < b->GetTime(); });

    for (size_t idx = 0; idx < hits.size(); ++idx) {
      hits[idx]->SetCrossingIndex(static_cast<G4int>(idx));
    }

    const auto* first = hits.front();
    const G4int nCrossings = static_cast<G4int>(hits.size());
    const G4double rB = first->GetRB();

    analysisManager->FillH2(6, rB, nCrossings);
    if (nCrossings >= 2) {
      analysisManager->FillH1(0, rB);
    } else {
      analysisManager->FillH1(1, rB);
    }

    const G4double time1 = first->GetTime();
    const G4double time1Smeared = SmearedCrossingTime(first, sigmaT);
    const G4double phi1 = first->GetPhi();
    const G4double theta1 = first->GetTheta();

    G4double phi2 = 0.;
    G4double theta2 = 0.;
    G4double time2 = 0.;
    G4double time2Smeared = 0.;
    G4double deltaPhi = 0.;
    G4double deltaTime = 0.;

    if (nCrossings >= 2) {
      const auto* second = hits[1];
      phi2 = second->GetPhi();
      theta2 = second->GetTheta();
      time2 = second->GetTime();
      time2Smeared = SmearedCrossingTime(second, sigmaT);
      deltaPhi = phi2 - phi1;
      while (deltaPhi > CLHEP::pi) deltaPhi -= 2. * CLHEP::pi;
      while (deltaPhi < -CLHEP::pi) deltaPhi += 2. * CLHEP::pi;
      deltaTime = time2Smeared - time1Smeared;
    }

    analysisManager->FillNtupleIColumn(1, 0, eventID);
    analysisManager->FillNtupleIColumn(1, 1, first->GetTrackID());
    analysisManager->FillNtupleIColumn(1, 2, first->GetHitLayer());
    analysisManager->FillNtupleIColumn(1, 3, first->GetPDG());
    analysisManager->FillNtupleIColumn(1, 4, nCrossings);
    analysisManager->FillNtupleDColumn(1, 5, first->GetPt());
    analysisManager->FillNtupleDColumn(1, 6, rB);
    analysisManager->FillNtupleDColumn(1, 7, phi1);
    analysisManager->FillNtupleDColumn(1, 8, theta1);
    analysisManager->FillNtupleDColumn(1, 9, time1);
    analysisManager->FillNtupleDColumn(1, 10, time1Smeared);
    analysisManager->FillNtupleDColumn(1, 11, phi2);
    analysisManager->FillNtupleDColumn(1, 12, theta2);
    analysisManager->FillNtupleDColumn(1, 13, time2);
    analysisManager->FillNtupleDColumn(1, 14, time2Smeared);
    analysisManager->FillNtupleDColumn(1, 15, deltaPhi);
    analysisManager->FillNtupleDColumn(1, 16, deltaTime);
    analysisManager->AddNtupleRow(1);
  }
}
}  // namespace

namespace B4a
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* /*event*/)
{
  fEnergyAbs = 0.;
  ChargedHits = 0.;
  TimeHits = 0.;
  fFirstHitRecorded = false;
  fTrajectory.clear();
}

void EventAction::AddTrajectoryPoint(G4int trackID, G4int pdg, G4double x, G4double y, G4double z)
{
  fTrajectory.push_back({trackID, pdg, x, y, z});
}

void EventAction::WriteTrajectory(G4int eventID)
{
  if (fTrajectory.empty()) return;
  auto* analysisManager = G4AnalysisManager::Instance();
  for (const auto& pt : fTrajectory) {
    analysisManager->FillNtupleIColumn(2, 0, eventID);
    analysisManager->FillNtupleIColumn(2, 1, pt.trackID);
    analysisManager->FillNtupleIColumn(2, 2, pt.pdg);
    analysisManager->FillNtupleDColumn(2, 3, pt.x);
    analysisManager->FillNtupleDColumn(2, 4, pt.y);
    analysisManager->FillNtupleDColumn(2, 5, pt.z);
    analysisManager->AddNtupleRow(2);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{
  if (hitColID == -1) {
    hitColID = G4SDManager::GetSDMpointer()->GetCollectionID("SiliconSD/SiliconHitsCollection");
  }

  auto* hce = event->GetHCofThisEvent();
  if (!hce || hitColID < 0) return;

  auto* hitsCol = const_cast<SiliconHitsCollection*>(
      static_cast<const SiliconHitsCollection*>(hce->GetHC(hitColID)));

  if (hitsCol) {
    ProcessCurlingHits(hitsCol, event->GetEventID(), fSigmaT);
  }

  WriteTrajectory(event->GetEventID());

  G4DigiManager* digiManager = G4DigiManager::GetDMpointer();
  digiManager->Digitize("SiliconDigitizer");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B4a
