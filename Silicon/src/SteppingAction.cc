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
/// \file B4/B4a/src/SteppingAction.cc
/// \brief Implementation of the B4a::SteppingAction class

#include "SteppingAction.hh"
#include "G4SDManager.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "SiliconHit.hh"
#include "G4Step.hh"
#include "globals.hh"
#include "G4EventManager.hh"

using namespace B4;

namespace B4a
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(const DetectorConstruction* detConstruction,
                               EventAction* eventAction)
  : fDetConstruction(detConstruction), fEventAction(eventAction)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  // Collect energy and track length step by step

  // get volume of the current step
  //auto volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  auto volume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
  auto volume_pre = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  G4Track* track = step->GetTrack();
  G4StepPoint* cPoint = step->GetPostStepPoint();
  G4StepPoint* pPoint = step->GetPreStepPoint();

  if (track->GetParentID() == 0 && (volume == fDetConstruction->GetsiliconSensorPV() && volume_pre != fDetConstruction->GetsiliconSensorPV()) || (volume == fDetConstruction->GetsiliconSensorPV_1() && volume_pre != fDetConstruction->GetsiliconSensorPV_1())) {
  //if (volume == fDetConstruction->GetsiliconSensorPV()) {


      // energy deposit
      auto edep = step->GetTotalEnergyDeposit();
      // get mass
      G4ParticleDefinition* particleDef = track->GetDefinition();
      G4double mass = particleDef->GetPDGMass();
      
      // get stepLength
      auto stepLength = step->GetStepLength();
      if (stepLength <= 0) return;

      auto vertex = track->GetVertexPosition();
      auto pos = cPoint->GetPosition(); // global position

      // get actual drift length on z
      G4TouchableHandle touchable = cPoint->GetTouchableHandle();
      G4VSolid* solid = touchable->GetVolume()->GetLogicalVolume()->GetSolid();
      G4ThreeVector localPos = touchable->GetHistory()->GetTopTransform().TransformPoint(pos); // change it to local position

      G4double distanceToZsurface;
          G4double halfZ = 100*um;
          G4double zLocal = localPos.z();  // current z position in local coordinates
          // 4. Distance to +Z or -Z surface
          distanceToZsurface = std::min(halfZ - zLocal, halfZ + zLocal);
      
         // G4cout << "Distance to Z surface: " << distanceToZsurface / mm << " mm" << G4endl;

      int hit_layer = (volume == fDetConstruction->GetsiliconSensorPV()) ? 0 : 1;
      //int hit_layer = pos.z()>1*cm ? 1:0;
      //auto pathLength = (pos-vertex).mag();
      auto pathLength = track-> GetTrackLength();

      auto time = cPoint->GetGlobalTime();
      auto momentum = cPoint->GetMomentum();
      auto pMag = momentum.mag();

      auto dedx = edep / stepLength;
 
      static G4int hcID = -1;
      if (hcID < 0) {
          hcID = G4SDManager::GetSDMpointer()->GetCollectionID("SiliconSD/SiliconHitsCollection");
      }
      if (hcID < 0) {
        G4cerr << "[FATAL] SiliconHitsCollection not registered yet!" << G4endl;
        return;
      }

      G4HCofThisEvent* hce = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetHCofThisEvent();
      if (!hce) return;
      auto* hitCollection = static_cast<SiliconHitsCollection*>(hce->GetHC(hcID));
      if (!hitCollection) return;


        fEventAction->AddAbs(edep);
          // Create and fill hit
          auto* hit = new SiliconHit();
          hit->SetEdep(edep);
          hit->SetPos(pos);
          hit->SetVertex(vertex);
          hit->SetPathLength(pathLength);

          hit->SetTime(time);
          hit->SetStepLength(stepLength);
          hit->SetDedx(dedx);
          hit->SetMomIn(pMag);
          hit->SetParticleMass(mass);
          hit->SetActualDriftz(distanceToZsurface);

          hit->SetHitLayer(hit_layer);
          hitCollection->insert(hit);
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B4a
