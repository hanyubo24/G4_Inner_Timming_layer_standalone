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
#include "G4Tubs.hh"
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
#include <algorithm>
namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction(const std::vector<G4double>& radii, const G4double thickness,
                                           G4double bField,
                                           const std::vector<PassiveLayer>& passiveLayers)
  : fRadii(radii), fthickness(thickness), fBField(bField), fPassiveLayers(passiveLayers)
{}

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
  
  G4double pitch = 100 * micrometer;
  G4double thickness = fthickness;     // already in G4 internal length units (mm)
  //G4double thickness = 2*cm;
  //auto worldSizeXY = 50* pitch;
  //auto worldSizeZ = worldSizeXY*10;
  //auto worldSizeXY = 500* pitch;
  // Large world so low-pT tracks can complete full curls and re-enter silicon
  const G4double radius_inner = fRadii[0] * mm;
  const G4double theta_min    = 20. * deg;
  // Each cylinder ends at z = R / tan(theta_min) so all layers cover the same
  // theta acceptance [theta_min, pi - theta_min]; world Z is sized for the
  // largest-radius element (silicon or passive shell).
  const G4double half_length_z = radius_inner / std::tan(theta_min);
  G4double max_radius_mm = *std::max_element(fRadii.begin(), fRadii.end());
  for (const auto& pl : fPassiveLayers)
    max_radius_mm = std::max(max_radius_mm, pl.radius_mm + pl.thickness_um * 1e-3);
  const G4double half_length_z_outer = max_radius_mm * mm / std::tan(theta_min);
  auto worldSizeXY = 3000 * cm;
  auto worldSizeZ  = half_length_z_outer * 2 * 3;

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

  worldLV->SetVisAttributes(G4VisAttributes::GetInvisible());

  
  // Silicon cylinder coaxial with Z (Belle II style), centred at the origin.
  // G4Tubs: inner radius, outer radius, half-length along Z.
  // Cover a fixed polar-angle acceptance [theta_min, 180-theta_min] for all radii.
  // At radius R: z_max = R / tan(theta_min)  (same solid-angle slice regardless of R)

  auto solidSensor = new G4Tubs("Silicon", radius_inner, radius_inner + thickness,
                                half_length_z, 0.0 * deg, 360.0 * deg);
  auto siliconSensorLV = new G4LogicalVolume(solidSensor, siliconMaterial, "Silicon");
  fSiliconLogic = siliconSensorLV;
  siliconSensorPV = new G4PVPlacement(nullptr, G4ThreeVector(), siliconSensorLV, "Silicon",
                                      worldLV, false, 0, fCheckOverlaps);
  siliconSensorLV->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));

  if (fRadii.size() > 1) {
    const G4double radius_inner_layer1 = fRadii[1] * mm;
    const G4double half_length_z_l1 = radius_inner_layer1 / std::tan(theta_min);
    auto solidSensor_layer1 = new G4Tubs("Silicon_layer1", radius_inner_layer1,
                                         radius_inner_layer1 + thickness, half_length_z_l1,
                                         0.0 * deg, 360.0 * deg);
    auto siliconSensorLV_layer1 = new G4LogicalVolume(solidSensor_layer1, siliconMaterial, "Silicon_layer1");
    fSiliconLogic_layer1 = siliconSensorLV_layer1;
    siliconSensorPV_layer1 = new G4PVPlacement(nullptr, G4ThreeVector(), siliconSensorLV_layer1,
                                               "Silicon_layer1", worldLV, false, 0, fCheckOverlaps);
    siliconSensorLV_layer1->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));
  }
  G4double minStep = 0.0 * um;

  auto* userLimits = new G4UserLimits(0., DBL_MAX, DBL_MAX, 0., minStep);
  siliconSensorLV->SetUserLimits(userLimits);
//  siliconSensorLV_1->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));

  // ── Passive material shells (beampipe / CDC-wall mock-ups) ───────────────
  // Non-sensitive cylinders that scatter and lose energy but produce no hits.
  // Each spans the same theta acceptance as the silicon layers.
  auto* nist = G4NistManager::Instance();
  G4int ipass = 0;
  for (const auto& pl : fPassiveLayers) {
    auto* mat = nist->FindOrBuildMaterial(pl.material);
    if (!mat) {
      G4cerr << "[DetectorConstruction] unknown passive material '" << pl.material
             << "' — skipping layer '" << pl.label << "'" << G4endl;
      continue;
    }
    const G4double r_in  = pl.radius_mm * mm;
    const G4double r_out = r_in + pl.thickness_um * um;
    const G4double half_z = pl.radius_mm * mm / std::tan(theta_min);
    const G4String name = "Passive_" + std::to_string(ipass) + "_" + pl.label;

    auto* solid = new G4Tubs(name, r_in, r_out, half_z, 0.0 * deg, 360.0 * deg);
    auto* lv    = new G4LogicalVolume(solid, mat, name);
    new G4PVPlacement(nullptr, G4ThreeVector(), lv, name, worldLV, false, 0, fCheckOverlaps);
    lv->SetVisAttributes(G4VisAttributes(G4Colour::Gray()));

    const G4double x0frac = pl.thickness_um * um / mat->GetRadlen() * 100.;
    G4cout << "[DetectorConstruction] passive '" << pl.label << "': "
           << pl.material << "  R=" << pl.radius_mm << " mm  t=" << pl.thickness_um
           << " um  -> " << x0frac << " % X0" << G4endl;
    ++ipass;
  }

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
  G4ThreeVector fieldValue = G4ThreeVector(0., 0., fBField * tesla);

  //G4ThreeVector fieldValue = G4ThreeVector(0., 0., 0.);
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);

  G4SDManager* sdManager = G4SDManager::GetSDMpointer();
  auto* siliconSD = new SiliconSensitiveDetector("SiliconSD");
  sdManager->AddNewDetector(siliconSD);
  fSiliconLogic->SetSensitiveDetector(siliconSD);
  if (fRadii.size()>1) {
  fSiliconLogic_layer1->SetSensitiveDetector(siliconSD);
  }
 // fSiliconLogic_1->SetSensitiveDetector(siliconSD);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B4
