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
/// \file B4/B4a/src/DetectorConstruction.cc
/// \brief Implementation of the B4::DetectorConstruction class

#include "DetectorConstruction.hh"
#include "SiliconSensitiveDetector.hh"
#include "G4AutoDelete.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4SDManager.hh"
#include "G4UserLimits.hh"
namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // Lead material defined using NIST Manager
  auto nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Si");
  nistManager->FindOrBuildMaterial("G4_AIR");
  nistManager->FindOrBuildMaterial("G4_Galactic");

  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
  // Geometry parameters
  G4double pitch = 100 * micrometer;
  G4double thickness = 200 * micrometer;
  //auto worldSizeXY = 50* pitch;
  //auto worldSizeZ = worldSizeXY*10;
  //auto worldSizeXY = 500* pitch;
  auto worldSizeXY = 60*cm;
  auto worldSizeZ = 60 * cm;

  auto siliconMaterial = G4Material::GetMaterial("G4_Si");
  auto air = G4Material::GetMaterial("G4_AIR");
  auto vacuum = G4Material::GetMaterial("G4_Galactic");
  //
  // World
  //
  auto worldS = new G4Box("World",  // its name
                          worldSizeXY / 2, worldSizeXY / 2, worldSizeZ / 2);  // its size

  auto worldLV = new G4LogicalVolume(worldS,  // its solid
                                     vacuum,  // its material
                                     "World");  // its name

  auto worldPV = new G4PVPlacement(nullptr,  // no rotation
                                   G4ThreeVector(),  // at (0,0,0)
                                   worldLV,  // its logical volume
                                   "World",  // its name
                                   nullptr,  // its mother  volume
                                   false,  // no boolean operation
                                   0,  // copy number
                                   fCheckOverlaps);  // checking overlaps

  //
  // Silicon
  auto solidSensor = new G4Box("Silicon", 400*pitch/2, 100*pitch/2, thickness/2);
  auto siliconSensorLV = new G4LogicalVolume(solidSensor, siliconMaterial, "Silicon");

  fSiliconLogic = siliconSensorLV; 
  siliconSensorPV = new G4PVPlacement(0, G4ThreeVector(0,0,thickness/2), siliconSensorLV, "Silicon", worldLV, false, 0, fCheckOverlaps);

  auto solidSensor_1 = new G4Box("Silicon_1", 400*pitch/2, 100*pitch/2, thickness/2);
  auto siliconSensorLV_1 = new G4LogicalVolume(solidSensor_1, siliconMaterial, "Silicon_1");

  fSiliconLogic_1 = siliconSensorLV_1; 
  siliconSensorPV_1 = new G4PVPlacement(0, G4ThreeVector(0,0,10*cm), siliconSensorLV_1, "Silicon_1", worldLV, false, 0, fCheckOverlaps);

  // Visualization attributes
  //
  worldLV->SetVisAttributes(G4VisAttributes::GetInvisible());
  //worldLV->SetVisAttributes(G4VisAttributes(G4Colour::White()));
  siliconSensorLV->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));
  siliconSensorLV_1->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));

  // Always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  //G4ThreeVector fieldValue = G4ThreeVector(0., 1.5*tesla, 0.);
  G4ThreeVector fieldValue = G4ThreeVector(0., 0., 0.);
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);

  G4SDManager* sdManager = G4SDManager::GetSDMpointer();
  auto* siliconSD = new SiliconSensitiveDetector("SiliconSD");
  sdManager->AddNewDetector(siliconSD);
  fSiliconLogic->SetSensitiveDetector(siliconSD);
  fSiliconLogic_1->SetSensitiveDetector(siliconSD);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B4
