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
#include <cmath>

namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(const G4String& particleName, G4double pMin, G4double pMax,
                                               G4double gunZ, G4double fixedTheta, G4double fixedPhi,
                                               G4double fixedPt, G4double fixedPz,
                                               G4double ptMin, G4double ptMax,
                                               G4double cosThMin, G4double cosThMax)
:fParticleName(particleName), fPMin(pMin), fPMax(pMax), fgunZ(gunZ),
 fFixedTheta(fixedTheta), fFixedPhi(fixedPhi), fFixedPt(fixedPt), fFixedPz(fixedPz),
 fPtMin(ptMin), fPtMax(ptMax), fCosThMin(cosThMin), fCosThMax(cosThMax)
{
  G4int nofParticles = 1;
  fParticleGun = new G4ParticleGun(nofParticles);
  useFile = false;
}

PrimaryGeneratorAction::PrimaryGeneratorAction(const G4String& filename, G4double gunZ)
{
    G4int nofParticles = 1;
    fParticleGun = new G4ParticleGun(nofParticles);
    fgunZ =  gunZ;
    finFile.open(filename);
    if (!finFile.is_open()){
        G4Exception("PrimaryGeneratorAction", "FileOpenError", FatalException,
                    ("Cannot open input file: " + filename).c_str());

    }
   useFile = true;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  if (useFile){
       finFile.close();
   }
  delete fParticleGun;
 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  // reading from a file
  double cth, phi, p0, theta;
  G4double p = 0.;
  G4ThreeVector dir;

  if (useFile) {
        std::string line;
        while (std::getline(finFile, line)) {
            if (line.empty() || !isdigit(line[0])) continue;

            std::stringstream ss(line);
            std::string token;

            G4int evt, run, pdg, primary;

            try {
                std::getline(ss, token, ','); evt = std::stoi(token);
                std::getline(ss, token, ','); run = std::stoi(token);
                std::getline(ss, token, ','); p0 = std::stod(token); // GeV
                std::getline(ss, token, ','); cth = std::stod(token);
                std::getline(ss, token, ','); phi = std::stod(token); // rad
                std::getline(ss, token, ','); pdg = std::stoi(token); 
                std::getline(ss, token, ','); theta = std::stoi(token); // rad
                std::getline(ss, token, ','); primary = std::stoi(token); 
                p = p0 * GeV;
                
                           
            } catch (...) {
                G4cerr << "Bad line in input file: " << line << G4endl;
                continue;
            }
            if (pdg ==0){continue;} 
            if (!(abs(pdg) == 321 || abs(pdg) == 211 || abs(pdg) == 11 || abs(pdg) == 13 || abs(pdg) ==2212)) {continue;}
            if (p < 10 * MeV){continue;}
            if (primary ==0){continue;}
            auto particleDefinition = G4ParticleTable::GetParticleTable()->FindParticle(pdg);
            fParticleGun->SetParticleDefinition(particleDefinition);
           
            if (!particleDefinition) {
                G4cerr << "Unknown PDG code: " << pdg << G4endl;
                continue;
            }

            G4double px = p * std::sin(theta) * std::cos(phi);
            G4double py = p * std::sin(theta) * std::sin(phi);
            G4double pz = p * cth;

            auto dir = G4ThreeVector(px, py, pz).unit();
            break; // generate only one particle per event
        }

        if (finFile.eof()) {
            G4cerr << "End of file reached. No more particles." << G4endl;
            useFile = false; // switch to fallback mode
        }
    }
    if (!useFile) {
        auto particleDefinition = G4ParticleTable::GetParticleTable()->FindParticle(fParticleName);
        fParticleGun->SetParticleDefinition(particleDefinition);

        if (fPtMin >= 0. && fPtMax > fPtMin) {
          // Flat-in-pT mode: uniform pT, uniform phi, uniform cos(theta)
          const G4double pt     = fPtMin + (fPtMax - fPtMin) * G4UniformRand();
          const G4double phi_az = CLHEP::twopi * G4UniformRand();
          const G4double costh  = fCosThMin + (fCosThMax - fCosThMin) * G4UniformRand();
          const G4double sinth  = std::sqrt(1. - costh * costh);
          const G4double px = pt * std::cos(phi_az);
          const G4double py = pt * std::sin(phi_az);
          const G4double pz = pt * costh / sinth;
          const G4ThreeVector mom(px, py, pz);
          p   = mom.mag();
          dir = mom.unit();
        } else if (fFixedPt >= 0. && fFixedPz >= 0.) {
          const G4double phi_az = (fFixedPhi >= 0.) ? fFixedPhi : CLHEP::twopi * G4UniformRand();
          const G4ThreeVector mom(fFixedPt * std::cos(phi_az), fFixedPt * std::sin(phi_az), fFixedPz);
          p = mom.mag();
          dir = mom.unit();
        } else if (fFixedTheta >= 0. && fFixedPhi >= 0.) {
          p = fPMin + (fPMax - fPMin) * G4UniformRand();
          dir = G4ThreeVector(std::sin(fFixedTheta) * std::cos(fFixedPhi),
                              std::sin(fFixedTheta) * std::sin(fFixedPhi),
                              std::cos(fFixedTheta));
        } else {
          p = fPMin + (fPMax - fPMin) * G4UniformRand();
          G4double Mdx = G4UniformRand() - 0.5;
          G4double Mdy = G4UniformRand() - 0.5;
          G4double Mdz = G4UniformRand() - 0.5;
          dir = G4ThreeVector(Mdx, Mdy, Mdz).unit();
        }
    }

    fParticleGun->SetParticleMomentum(p);
    fParticleGun->SetParticleMomentumDirection(dir);

    fParticleGun->SetParticlePosition(G4ThreeVector(0., 0.,  fgunZ));
    fParticleGun->GeneratePrimaryVertex(event);

  // Set gun position
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B4
