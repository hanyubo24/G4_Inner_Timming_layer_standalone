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
/// \file B4/B4a/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the B4::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include "Randomize.hh"

namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(const G4String& particleName, G4double pMin, G4double pMax, G4double gunZ )
:fParticleName(particleName), fPMin(pMin), fPMax(pMax), fgunZ(gunZ)
{
  G4int nofParticles = 1;
  
  fParticleGun = new G4ParticleGun(nofParticles);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  // This function is called at the begining of event

  // In order to avoid dependence of PrimaryGeneratorAction

  G4double p = fPMin + (fPMax - fPMin) * G4UniformRand();

  G4double Mdx = (0.1)* G4UniformRand()-0.1; 
  G4double Mdy =  (0.05)* G4UniformRand()-0.025; 
  
  auto particleDefinition = G4ParticleTable::GetParticleTable()->FindParticle(fParticleName);
  G4cout << "[PrimaryGenerator] Using " << fParticleName 
       << " with p in range: [" << fPMin / MeV << " MeV, " 
       << fPMax / MeV << " MeV]" << G4endl;
  G4cout << "[PrimaryGenerator] position z: (should be always negative value ) " << fgunZ <<G4endl; 
  G4double mass = particleDefinition->GetPDGMass();
  G4double energy = std::sqrt(p * p + mass * mass);

  fParticleGun->SetParticleDefinition(particleDefinition);
  fParticleGun->SetParticleEnergy(energy);
  //fParticleGun->SetParticleMomentumDirection(G4ThreeVector(Mdx, Mdy, 1.));
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1.));


 // G4double worldZHalfLength = 0.;
 // auto worldLV = G4LogicalVolumeStore::GetInstance()->GetVolume("World");

 // auto siliconSensorLV = G4LogicalVolumeStore::GetInstance()->GetVolume("Silicon");
 // 
 // // Check that the world volume has box shape
 // G4Box* worldBox = nullptr;
 // if (siliconSensorLV) {
 //   worldBox = dynamic_cast<G4Box*>(siliconSensorLV->GetSolid());
 // }

 // if (worldBox) {
 //   worldZHalfLength = worldBox->GetZHalfLength();
 // }
 // else {
 //   G4ExceptionDescription msg;
 //   msg << "World volume of box shape not found." << G4endl;
 //   msg << "Perhaps you have changed geometry." << G4endl;
 //   msg << "The gun will be place in the center.";
 //   G4Exception("PrimaryGeneratorAction::GeneratePrimaries()", "MyCode0002", JustWarning, msg);
 // }
  fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., fgunZ));
  fParticleGun->GeneratePrimaryVertex(event);

  // Set gun position
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B4
