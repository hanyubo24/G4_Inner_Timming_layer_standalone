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
/// \file B4/B4a/src/RunAction.cc
/// \brief Implementation of the B4::RunAction class

#include "RunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "globals.hh"
#include "G4DigiManager.hh"
#include "G4AutoLock.hh"
namespace {
    G4Mutex analysisFileMutex = G4MUTEX_INITIALIZER;
}

namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(const G4String& outFileName)
: fOutFileName(outFileName)
{
  // set printing event number per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);


  // Create analysis manager
  // The choice of the output format is done via the specified
  // file extension.
 // auto analysisManager = G4AnalysisManager::Instance();

  // Create directories
  // analysisManager->SetHistoDirectoryName("histograms");
  // analysisManager->SetNtupleDirectoryName("ntuple");
  //analysisManager->SetVerboseLevel(1);
  //analysisManager->SetNtupleMerging(true);
  // Note: merging ntuples is available only with Root output

  // Book histograms, ntuple
    auto* analysisManager = G4AnalysisManager::Instance();
    //analysisManager->SetVerboseLevel(1);
    //analysisManager->SetNtupleMerging(true);

    analysisManager->CreateH2("HitMap_layer0", "HitMap;PixID X;PixID Y", 400, -200,200, 100,-50, 50 );
    analysisManager->CreateH2("Dedx_hit", "Dedx per hit:P;Dedx", 500,0,3,500,0,20);
    analysisManager->CreateH2("Dedx_event", "Dedx per event;P;Dedx ", 500,0,3,500,0,20);
    analysisManager->CreateH2("Dedx_hit_gb", "Dedx per hit;[betaGamma];Dedx ", 500,0,10,500,0,20);
    analysisManager->CreateH2("Dedx_event_bg", "Dedx per event;[betaGamma];Dedx ", 500,0,10,500,0,20);
    analysisManager->CreateH2("HitMap_layer1", "HitMap;PixID X;PixID Y", 400, -200,200, 100,-50, 50 );

    G4cout << "[THREAD] Creating ntuple..." << G4endl;
    analysisManager->CreateNtuple("HitNtuple", "Digitized Signals");
    analysisManager->CreateNtupleDColumn("charge"); 
    analysisManager->CreateNtupleDColumn("driftTime_notSmeared"); // drift to the surface of the silicon
    analysisManager->CreateNtupleDColumn("driftTime_Smeared"); 
    analysisManager->CreateNtupleDColumn("PixIDX");
    analysisManager->CreateNtupleDColumn("PixIDY");
    analysisManager->CreateNtupleDColumn("EnergyDep");
    analysisManager->CreateNtupleDColumn("StepLength"); 
    analysisManager->CreateNtupleDColumn("Dedx");
    analysisManager->CreateNtupleDColumn("MomIn");
    analysisManager->CreateNtupleDColumn("MassIn");
    analysisManager->CreateNtupleDColumn("betagamma");
    analysisManager->CreateNtupleDColumn("layer");
    analysisManager->CreateNtupleDColumn("Gtime"); // global time
    analysisManager->CreateNtupleDColumn("Gtime_smeared"); // Gtime + driftTime_Smeared
    analysisManager->CreateNtupleDColumn("P_e");
    analysisManager->CreateNtupleDColumn("P_mu");
    analysisManager->CreateNtupleDColumn("P_pi");
    analysisManager->CreateNtupleDColumn("P_kaon");
    analysisManager->CreateNtupleDColumn("P_p");
    analysisManager->CreateNtupleDColumn("HitPathLength_fromTrack");
    //analysisManager->CreateNtupleDColumn("EventID");

    analysisManager->FinishNtuple();
    analysisManager->CreateNtuple("EventNtuple", "Event signal");
    analysisManager->CreateNtupleDColumn("NumberOfHits");
    analysisManager->CreateNtupleDColumn("Charge");
    analysisManager->CreateNtupleDColumn("TotalLength");
    analysisManager->CreateNtupleDColumn("Dedx");
    analysisManager->CreateNtupleDColumn("betagamma");
    analysisManager->FinishNtuple();
  //
  // Creating histograms
  //analysisManager->CreateH1("Eabs", "Edep in absorber", 110, 0., 30 * MeV);
  //analysisManager->CreateH1("nCharge", "Number of Charges in one event", 100, 0., 100);
  //analysisManager->CreateH1("TimeHits", "Time of the hit", 100, 0., 100);

  // Creating ntuple
  //
  //analysisManager->CreateNtuple("Silicon", "Edep");
  //analysisManager->CreateNtupleDColumn("Eabs");
  //analysisManager->CreateNtupleDColumn("nCharge");
  //analysisManager->CreateNtupleDColumn("TimeHits");
  //analysisManager->CreateNtupleDColumn("Waveform");
  //analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* /*run*/)
{
  // inform the runManager to save random number seed
  // G4RunManager::GetRunManager()->SetRandomNumberStore(true);

  // Get analysis manager
    auto* analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetVerboseLevel(1);
    analysisManager->SetNtupleMerging(true); 
     
    analysisManager->OpenFile(fOutFileName);  // 

    //
 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* /*run*/)
{
        auto* analysisManager = G4AnalysisManager::Instance();
        analysisManager->Write();
        analysisManager->CloseFile(false);
        G4cout << "[RunAction] Closing output file..." << G4endl;
}
 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B4
