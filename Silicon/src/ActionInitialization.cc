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
/// \file B4/B4a/src/ActionInitialization.cc
/// \brief Implementation of the B4a::ActionInitialization class

#include "ActionInitialization.hh"

#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"
#include "G4DigiManager.hh"
#include "SiliconDigitizer.hh"

using namespace B4;

namespace B4a
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ActionInitialization::ActionInitialization(DetectorConstruction* detConstruction, const G4String& particleName, G4double pMin, G4double pMax, const G4String& outFileName, G4double gunZ)
  : fDetConstruction(detConstruction),
    fParticleName(particleName),
    fPMin(pMin),
    fPMax(pMax),
    foutFileName(outFileName),
    fgunZ(gunZ)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::BuildForMaster() const
{
 // auto* analysisManager = G4AnalysisManager::Instance();
 //   analysisManager->SetVerboseLevel(1);
 //   analysisManager->SetNtupleMerging(true);
 //   analysisManager->OpenFile("Silicon.root");  
  SetUserAction(new RunAction(foutFileName));
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::Build() const
{
  SetUserAction(new PrimaryGeneratorAction(fParticleName, fPMin, fPMax, fgunZ));
  SetUserAction(new RunAction(foutFileName));
  auto eventAction = new EventAction;
  SetUserAction(eventAction);
  SetUserAction(new SteppingAction(fDetConstruction, eventAction));

  auto* digiManager = G4DigiManager::GetDMpointer();
  auto* digitizer = new SiliconDigitizer("SiliconDigitizer");
  digiManager->AddNewModule(digitizer);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B4a
