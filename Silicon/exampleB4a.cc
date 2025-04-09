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
/// \file exampleB4a.cc
/// \brief Main program of the B4a example

#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "FTFP_BERT.hh"
#include "G4EmParameters.hh"
#include "G4RunManagerFactory.hh"
#include "G4SteppingVerbose.hh"
#include "G4UIExecutive.hh"
#include "G4UIcommand.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4DigiManager.hh"
#include "SiliconDigitizer.hh"
// #include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

namespace
{
void PrintUsage()
{
  G4cerr << " Usage: " << G4endl;
  G4cerr << " exampleB4a [-m macro ] [-u UIsession] [-t nThreads] [-vDefault]" << G4endl;
  G4cerr << " exampleB4a -p e- -pmin 80 -pmax 80 -o myOutput.root -z -14.0" << G4endl;
  G4cerr << " -ui: turnning on the ui" << G4endl;
}
}  // namespace

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc, char** argv)
{
  // Evaluate arguments
  //
  if (argc > 1 && G4String(argv[1]) == "--h") {
  PrintUsage();
  return 0;
}

  G4String macro;
  G4String session;
  G4bool verboseBestUnits = true;
#ifdef G4MULTITHREADED
  G4int nThreads = 0;
  G4double pMin = 80 *MeV;
  G4double pMax = 80 *MeV;
  G4String particleName = "e-";
  G4bool show_gui = false; 
  G4String outFileName = "Silicon.root"; 
  G4int nEvent = 1000;
  G4double gunZPos = -14.0 * cm; 
#endif
  for (G4int i = 1; i < argc; i = i + 2) {
    if (G4String(argv[i]) == "-m")
      macro = argv[i + 1];
    else if (G4String(argv[i]) == "-n")
      nEvent = G4UIcommand::ConvertToInt(argv[i + 1]);
    else if (G4String(argv[i]) == "-u")
      session = argv[i + 1];
    else if (G4String(argv[i]) == "-z")
      gunZPos = std::stod(argv[i + 1]) * CLHEP::cm;
    else if (G4String(argv[i]) == "-p")
        particleName = argv[i + 1];
    else if (G4String(argv[i]) == "-pmin")
        pMin = std::stod(argv[i + 1]) * MeV;
    else if (G4String(argv[i]) == "-pmax")
        pMax = std::stod(argv[i + 1]) * MeV;
    else if (G4String(argv[i]) == "-o")  
        outFileName = argv[i + 1];
    else if (G4String(argv[i]) == "-ui") 
        show_gui = true;
#ifdef G4MULTITHREADED
    else if (G4String(argv[i]) == "-t") {
      nThreads = G4UIcommand::ConvertToInt(argv[i + 1]);
    }
#endif
    else if (G4String(argv[i]) == "-vDefault") {
      verboseBestUnits = false;
      --i;  // this option is not followed with a parameter
    }
    else {
      PrintUsage();
      return 1;
    }
  }

  // Detect interactive mode (if no macro provided) and define UI session
  //
  G4UIExecutive* ui = nullptr;
  if (!macro.size()) {
    ui = new G4UIExecutive(argc, argv, session);
  }

  // Optionally: choose a different Random engine...
  // G4Random::setTheEngine(new CLHEP::MTwistEngine);

  // Use G4SteppingVerboseWithUnits
  if (verboseBestUnits) {
    G4int precision = 4;
    G4SteppingVerbose::UseBestUnit(precision);
  }

  // Construct the default run manager
  //
  auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
#ifdef G4MULTITHREADED
  if (nThreads > 0) {
    runManager->SetNumberOfThreads(nThreads);
  }
#endif

  // Set mandatory initialization classes
  //
  auto detConstruction = new B4::DetectorConstruction();
  runManager->SetUserInitialization(detConstruction);

  auto physicsList = new FTFP_BERT; // QGSP_BERT // YB: to be tested
  runManager->SetUserInitialization(physicsList);

  //auto actionInitialization = new B4a::ActionInitialization(detConstruction);
  auto actionInitialization = new B4a::ActionInitialization(detConstruction, particleName, pMin, pMax, outFileName, gunZPos);
  runManager->SetUserInitialization(actionInitialization);

  runManager-> Initialize();
  auto digimgr = G4DigiManager::GetDMpointer();
  auto* digiModule = new SiliconDigitizer("SiliconDigitizer");
  digimgr->AddNewModule(digiModule);
 
  G4EmParameters::Instance()->SetStepFunction(0.1, 0.5 * um);

  // Initialize visualization
  //
  auto visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // auto visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  auto UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  //
  if (macro.size()) {
    // batch mode
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command + macro);
  }
  else {
    G4RunManager::GetRunManager()->BeamOn(nEvent);

    if (show_gui){

        // interactive mode : define UI session
        
        UImanager->ApplyCommand("/control/execute init_vis.mac");
        if (ui->IsGUI()) {
          UImanager->ApplyCommand("/control/execute gui.mac");
        }
        ui->SessionStart();
        delete ui;
    }
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !

  delete visManager;
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
