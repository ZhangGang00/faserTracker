#include "FaserTracker/TrackingGeometry.hh"
#include "FaserTracker/common_includes.hh"
#include "FaserTrackerGeometry.hh"

#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Detector/TrackingVolume.hpp"
#include "Acts/Layers/PlaneLayer.hpp"
//#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/Material/HomogeneousSurfaceMaterial.hpp"
#include "Acts/Material/Material.hpp"
#include "Acts/Surfaces/PlaneSurface.hpp"
#include "Acts/Surfaces/RectangleBounds.hpp"
#include "Acts/Surfaces/SurfaceArray.hpp"
#include "Acts/Tools/LayerArrayCreator.hpp"
#include "Acts/Utilities/BinnedArrayXD.hpp"
#include "Acts/Utilities/BinningType.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Units.hpp"
#include "Acts/Volumes/VolumeBounds.hpp"
#include "Acts/Volumes/CuboidVolumeBounds.hpp"
#include "Acts/Material/MaterialProperties.hpp"

//#include "Acts/EventData/TrackParameters.hpp"
//#include "Acts/EventData/Measurement.hpp"
//#include "Acts/Fitter/KalmanFitter.hpp"
//#include "Acts/Fitter/KalmanFitterExtrapolator.hpp"
//#include "Acts/Fitter/KalmanCacheGenerator.hpp"
//#include "Acts/Fitter/KalmanCalibrator.hpp"
//#include "Acts/Fitter/KalmanUpdator.hpp"
//#include "../KalmanFitterUtils.hpp"

//#include "TROOT.h"
//#include "TSystem.h"
//#include "TChain.h"
//#include "TBranch.h"

//#include <fstream>
//#include <stdio.h> 
//#include <stdlib.h>
//#include <algorithm>



namespace FaserTracker {

//------------------------------------------------------------------------------

TrackingGeometry::TrackingGeometry(shared_ptr<FaserTrackerGeometry> geo)
    : xyBounds {make_shared<const Acts::RectangleBounds>(geo->halfX, geo->halfY)}
{
}

//------------------------------------------------------------------------------

shared_ptr<Acts::TrackingGeometry> TrackingGeometry::actsGeo() {

    Acts::Translation3D translation {0., 0., 0.};
    auto volumePos = make_shared<Acts::Transform3D>(translation);
    auto volumeBounds = make_shared<Acts::CuboidVolumeBounds>(geo->halfX, geo->halfY, geo->halfZ);
    //const auto volumeArray = make_shared<const Acts::TrackingVolumeArray>();

    //Acts::MutbableTrackingVolumePtr volume = TrackingVolume::create(volumePos, volumeBounds, volumeArray, "faser_tracking_volume");
    //return make_shared<Acts::TrackingGeometry>(volume);
    return nullptr;



    cout << "INFO  FaserTracker::TrackingGeometry::actsGeo\n"
         << "      Building surfaces...\n";

    auto xyBounds = make_shared<const Acts::RectangleBounds>(geo->halfX, geo->halfY);

    buildSurfaces();

    cout << "      Building layers...\n";
    buildLayers();

    cout << "      Building tracking volume...\n";
    buildTrackingVolume();

    cout << "      Building vacuum...\n";
    buildVacuumVolumes();

    //glueVolumes(mtvp, vacArr);

    //cout << "      Building world...\n";
    //Acts::MutableTrackingVolumePtr mtvpWorld = buildWorld(mtvp, vacArr);

    //// Build tracking geometry
    //auto tGeo = make_shared<Acts::TrackingGeometry>(Acts::TrackingGeometry(mtvpWorld));

    //return tGeo;
}


////////////////////////////////////////////////////////////////////////////////
// Implementation details
////////////////////////////////////////////////////////////////////////////////


void TrackingGeometry::buildSurfaces() {

    surfaces.clear();

    // Simplified geometry for FASER space points

    for (double zPlane : geo->planeZ) {
        const Acts::Vector3D center = {0., 0., zPlane};
        const Acts::Vector3D normal = {0., 0., 1.};
        surfaces.push_back(make_shared<Acts::PlaneSurface>(center, normal));
    }

}

//------------------------------------------------------------------------------

void TrackingGeometry::buildLayers() {

    layers.clear();

    // Put surfaces into layers.

    for (size_t i=0; i<surfaces.size(); ++i) {

        const auto sur = surfaces[i];

        // Move the layer to its destination
        Acts::Transform3D t3dLay {Acts::Translation3D(0., 0., geo->planeZ[i])};

        auto surArray = std::make_unique<Acts::SurfaceArray>(sur.get());

        // Create layer
        layers.push_back(Acts::PlaneLayer::create(
                         std::make_shared<const Acts::Transform3D>(t3dLay),
                         xyBounds,
                         std::move(surArray),
                         geo->thickness));
    }

}

//------------------------------------------------------------------------------

//Acts::MutableTrackingVolumePtr TrackingGeometry::buildTrackingVolume(array<Acts::LayerPtr, nLayers> & layPtr) {
void  TrackingGeometry::buildTrackingVolume() {

    // Translate the volume
    Acts::Transform3D trans;
    trans = Acts::Translation3D(0., 0., 0.5*(geo->planeZ.front() + geo->planeZ.back()));
    std::shared_ptr<const Acts::Transform3D> htrans
        = std::make_shared<Acts::Transform3D>(trans);

    // Create volume
    Acts::VolumeBoundsPtr volBoundsPtr
        = std::make_shared<const Acts::CuboidVolumeBounds>(
                Acts::CuboidVolumeBounds(geo->halfX, geo->halfY, 0.5*geo->planeZ.back() + this->eps));

    // Add material
    auto mat = make_shared<const Acts::Material>(geo->X0, geo->L0, geo->A, geo->Z, geo->rho);

    // Collect layers
    Acts::LayerVector layVec;
    for (auto layer : layers) layVec.push_back(layer);


    // Create layer array -> navigation through the layers

    Acts::LayerArrayCreator layArrCreator(
            Acts::getDefaultLogger("LayerArrayCreator", Acts::Logging::VERBOSE));

    std::unique_ptr<const Acts::LayerArray> layArr(
            layArrCreator.layerArray(layVec,
                geo->planeZ.front() - this->eps,
                geo->planeZ.front() + geo->planeZ.back() + this->eps,
                Acts::BinningType::arbitrary,
                Acts::BinningValue::binZ));

    // Create tracking volume
    vacuumVolume = {Acts::TrackingVolume::create(htrans, volBoundsPtr, mat, std::move(layArr), layVec, {}, {}, "sensor")};

    vacuumVolume->sign(Acts::GeometrySignature::Global);

}

//------------------------------------------------------------------------------

void TrackingGeometry::buildVacuumVolumes() {

    vacuumArray.clear();

    // Repeat the steps of the tracking volume but with vacuum and without layers
    for (int iVac = 0; iVac < this->nVacua; iVac++) {
        // Use epsilon around first and last vacuum -> assure everything happens in
        // the world
        if (iVac == 0) {
            Acts::Transform3D transVac;
            transVac = Acts::Translation3D(
                    0., 0., (20. * iVac + 10.) * Acts::units::_m - 0.5*this->eps);  // hardcoded
            std::shared_ptr<const Acts::Transform3D> htransVac
                = std::make_shared<Acts::Transform3D>(transVac);

            Acts::VolumeBoundsPtr volBoundsPtrVac
                = std::make_shared<const Acts::CuboidVolumeBounds>(
                        Acts::CuboidVolumeBounds(
                            geo->halfX, geo->halfY, (geo->planeZ.front() / (double)nVacua + this->eps) / 2));

            Acts::MutableTrackingVolumePtr mtvpVac(
                    Acts::TrackingVolume::create(htransVac,
                        volBoundsPtrVac,
                        nullptr,
                        "Vacuum" + std::to_string(iVac)));

            mtvpVac->sign(Acts::GeometrySignature::Global);
            vacuumArray.push_back(mtvpVac);
            continue;
        }

        if (iVac == nVacua - 1) {
            Acts::Transform3D transVac;
            transVac = Acts::Translation3D(
                    0., 0., (20. * iVac + 10.) * Acts::units::_m + 0.5*this->eps);  // hardcode
            std::shared_ptr<const Acts::Transform3D> htransVac
                = std::make_shared<Acts::Transform3D>(transVac);

            Acts::VolumeBoundsPtr volBoundsPtrVac
                = std::make_shared<const Acts::CuboidVolumeBounds>(
                        Acts::CuboidVolumeBounds(
                            geo->halfX, geo->halfY, 0.5*(geo->planeZ.front() / (double)nVacua + this->eps)));

            Acts::MutableTrackingVolumePtr mtvpVac(
                    Acts::TrackingVolume::create(htransVac,
                        volBoundsPtrVac,
                        nullptr,
                        "Vacuum" + std::to_string(iVac)));

            mtvpVac->sign(Acts::GeometrySignature::Global);
            vacuumArray.push_back(mtvpVac);
            continue;
        }

        Acts::Transform3D transVac;
        transVac = Acts::Translation3D(
                0., 0., (20. * iVac + 10.) * Acts::units::_m);  // hardcode
        std::shared_ptr<const Acts::Transform3D> htransVac
            = std::make_shared<Acts::Transform3D>(transVac);

        Acts::VolumeBoundsPtr volBoundsPtrVac
            = std::make_shared<const Acts::CuboidVolumeBounds>(
                    Acts::CuboidVolumeBounds(
                        geo->halfX, geo->halfY, 0.5*(geo->planeZ.front() / (double)nVacua)));

        Acts::MutableTrackingVolumePtr mtvpVac(Acts::TrackingVolume::create(
                    htransVac, volBoundsPtrVac, nullptr, "Vacuum" + std::to_string(iVac)));

        mtvpVac->sign(Acts::GeometrySignature::Global);
        vacuumArray.push_back(mtvpVac);
    }

}

//------------------------------------------------------------------------------

void TrackingGeometry::glueVolumes() {

    size_t nVol = vacuumArray.size(); // should be equal to `nVacua`

    for (int iVac = 0; iVac < nVol; ++iVac) {
        if (iVac != nVol - 1)
            vacuumArray[iVac]->glueTrackingVolume(
                    Acts::BoundarySurfaceFace::positiveFaceXY,
                    vacuumArray[iVac + 1],
                    Acts::BoundarySurfaceFace::negativeFaceXY);
        if (iVac != 0)
            vacuumArray[iVac]->glueTrackingVolume(
                    Acts::BoundarySurfaceFace::negativeFaceXY,
                    vacuumArray[iVac - 1],
                    Acts::BoundarySurfaceFace::positiveFaceXY);
    }

    vacuumVolume->glueTrackingVolume(Acts::BoundarySurfaceFace::negativeFaceXY,
            vacuumArray[nVol - 1],
            Acts::BoundarySurfaceFace::positiveFaceXY);
    vacuumArray[nVol - 1]->glueTrackingVolume(
            Acts::BoundarySurfaceFace::positiveFaceXY,
            vacuumVolume,
            Acts::BoundarySurfaceFace::negativeFaceXY);
}

//------------------------------------------------------------------------------

void TrackingGeometry::buildWorld() {

    size_t nVacs = vacuumArray.size(); // should be equal to `nVacua`

    // Translation of the world
    Acts::Transform3D transWorld;
    transWorld = Acts::Translation3D(0., 0., 0.5*(geo->planeZ.front() + geo->planeZ.back()));
    std::shared_ptr<const Acts::Transform3D> htransWorld
        = std::make_shared<Acts::Transform3D>(transWorld);

    // Create world volume
    Acts::VolumeBoundsPtr volBoundsPtrWorld
        = std::make_shared<const Acts::CuboidVolumeBounds>(
                Acts::CuboidVolumeBounds(
                    geo->halfX, geo->halfY, (geo->planeZ.front() + geo->planeZ.back()) / 2 + this->eps));

    // Collect the position of the subvolumes
    std::vector<std::pair<std::shared_ptr<const Acts::TrackingVolume>,
        Acts::Vector3D>>
            tapVec;
    tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(
                vacuumVolume, Acts::Vector3D(0., 0., geo->planeZ.front() + geo->planeZ.back() / 2)));
    for (int iVac = 0; iVac < nVacs; iVac++) {
        if (iVac == 0) {
            tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(
                        vacuumArray[iVac],
                        Acts::Vector3D(0.,
                            0.,
                            (20. * iVac + 10.) * Acts::units::_m
                            - this->eps / 2)));  // hardcode
            continue;
        }
        if (iVac == nVacs - 1) {
            tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(
                        vacuumArray[iVac],
                        Acts::Vector3D(0.,
                            0.,
                            (20. * iVac + 10.) * Acts::units::_m
                            + this->eps / 2)));  // hardcode
            continue;
        }
        tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(
                    vacuumArray[iVac],
                    Acts::Vector3D(
                        0., 0., (20. * iVac + 10.) * Acts::units::_m)));  // hardcode
    }

    // Collect the boarders of the volumes for binning
    std::vector<float> binBoundaries = {-this->eps};
    for (int iVac = 1; iVac < nVacs; iVac++)
        binBoundaries.push_back((geo->planeZ.front() / (double)nVacs) * iVac);
    binBoundaries.push_back(geo->planeZ.front() - this->eps);
    binBoundaries.push_back(geo->planeZ.front() + geo->planeZ.back() + this->eps);

    Acts::BinningData binData(
            Acts::BinningOption::open, Acts::BinningValue::binZ, binBoundaries);
    std::unique_ptr<const Acts::BinUtility> bu(new Acts::BinUtility(binData));

    // Collect the volumes
    std::shared_ptr<const Acts::TrackingVolumeArray> trVolArr(
            new Acts::BinnedArrayXD<Acts::TrackingVolumePtr>(tapVec, std::move(bu)));

    // Create the world
    world = shared_ptr<Acts::TrackingVolume>(Acts::TrackingVolume::create(
                htransWorld, volBoundsPtrWorld, trVolArr, "world"));

    world->sign(Acts::GeometrySignature::Global);
}

//------------------------------------------------------------------------------

TrackingGeometry::~TrackingGeometry() = default;

//------------------------------------------------------------------------------
//
//}
//
//
//
//
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//
//
//// Number of detector layers
//const unsigned int nLayers = 12;
//// Length of a step limited to 25m -> multiple vacuum volumes will be glued
//// together
//const int nVacs = 20;
//
//struct configParams {
//
//    // Thickness of the detector layers
//    double thicknessSCT     = 0.32 * Acts::units::_mm;
//    double thicknessSupport = 3.3 * Acts::units::_mm;
//    double thickness        = 2 * thicknessSCT + thicknessSupport;
//    // Material of the strips
//    float X0  = 95.7;
//    float L0  = 465.2;
//    float A   = 28.03;
//    float Z   = 14.;
//    float Rho = 2.32e-3;
//    // Local position of the detector layers
//    std::array<double, nLayers> planeZ = {
//        -996.01 * Acts::units::_mm,
//        -946.01 * Acts::units::_mm,
//        -896.01 * Acts::units::_mm,
//        -144.949 * Acts::units::_mm,
//        -94.9492 * Acts::units::_mm,
//        -44.9492 * Acts::units::_mm,
//         5.05078 * Acts::units::_mm,
//         55.0508 * Acts::units::_mm,
//         105.051 * Acts::units::_mm,
//         896.01 * Acts::units::_mm,
//         946.01 * Acts::units::_mm,
//         996.01 * Acts::units::_mm
//    };
//
//    // Position of the detector
//    double posFirstSur = 400. * Acts::units::_m;
//    // Epsilon to ensure to keep the vertex and the layers inside the
//    // corresponding volumes
//    float eps = 10. * Acts::units::_mm;
//    // Lorentz angle
//    double lorentzAngle = 0.;
//    // Geometry of the strips
//    double       sensorGap    = 269. * Acts::units::_um;
//    unsigned int nStrips    = 1280;
//    double       pitch       = 0.0755 * Acts::units::_mm;
//    double       lengthStrip = 48.2 * Acts::units::_mm;
//    // x-/y-size of the setup
//    double halfX = nStrips * pitch / 2;
//    double halfY = lengthStrip + sensorGap / 2;
//    // Rotation of the strip plates
//    double stereoAngle = 0.026;
//
//    // Configuration of the particle gun
//    unsigned             nEvents = 10;
//    Acts::ConstantBField bField;
//    size_t nParticles      = 100;
//    std::array<double, 2> z0Range         = {{-eps / 2, eps / 2}};
//    std::array<double, 2> d0Range         = {{0., 0.15 * Acts::units::_m}};
//    std::array<double, 2> phiRange		= {{-M_PI, M_PI}};
//    std::array<double, 2> etaRange        = {{7., 15.}};
//    std::array<double, 2> ptRange         = {{0., 10. * Acts::units::_MeV}};
//    double mass            = 105.6 * Acts::units::_MeV;
//    double charge          = -Acts::units::_e;
//    int pID             = 13;
//
//    configParams() : bField(0., 0., 0.) {}
//
//    void readConfig(char* filename)
//    {
//        std::ifstream ifs(filename, std::fstream::in);
//        std::string line;
//        //Detector configuration
//        if(getline(ifs, line))
//            thicknessSCT = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_mm;
//        if(getline(ifs, line))
//            thicknessSupport = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_mm;
//        if(getline(ifs, line))
//            X0 = atof(line.substr(line.find(" ") + 1).c_str());
//        if(getline(ifs, line))
//            L0 = atof(line.substr(line.find(" ") + 1).c_str());
//        if(getline(ifs, line))
//            A = atof(line.substr(line.find(" ") + 1).c_str());
//        if(getline(ifs, line))
//            Z = atof(line.substr(line.find(" ") + 1).c_str());
//        if(getline(ifs, line))
//            Rho = atof(line.substr(line.find(" ") + 1).c_str());
//        if(getline(ifs, line))
//            planeZ[0] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_mm;
//        if(getline(ifs, line))
//            planeZ[1] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_mm;
//        if(getline(ifs, line))
//            planeZ[2] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_mm;
//        if(getline(ifs, line))
//            planeZ[3] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_mm;
//        if(getline(ifs, line))
//            planeZ[4] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_mm;
//        if(getline(ifs, line))
//            planeZ[5] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_mm;
//        if(getline(ifs, line))
//            planeZ[6] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_mm;
//        if(getline(ifs, line))
//            planeZ[7] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_mm;
//        if(getline(ifs, line))
//            planeZ[8] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_mm;
//        if(getline(ifs, line))
//            planeZ[9] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_mm;
//        if(getline(ifs, line))
//            planeZ[10] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_mm;
//        if(getline(ifs, line))
//            planeZ[11] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_mm;
//        if(getline(ifs, line))
//            posFirstSur = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_m;
//        if(getline(ifs, line))
//            eps = atof(line.substr(line.find(" ") + 1).c_str()) * Acts::units::_mm;
//        if(getline(ifs, line))
//            lorentzAngle = strtod(line.substr(line.find(" ") + 1).c_str(), NULL);
//        if(getline(ifs, line))
//            sensorGap = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_um;
//        if(getline(ifs, line))
//            nStrips = atoi(line.substr(line.find(" ") + 1).c_str());
//        if(getline(ifs, line))
//            pitch = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_um;
//        if(getline(ifs, line))
//            lengthStrip = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_mm;
//        if(getline(ifs, line))
//            rotation = strtod(line.substr(line.find(" ") + 1).c_str(), NULL);
//        // Particle gun configuration
//        if(getline(ifs, line))
//            nEvents = atoi(line.substr(line.find(" ") + 1).c_str());
//        Acts::Vector3D bfieldTmp(0., 0., 0.);
//        if(getline(ifs, line))
//            bfieldTmp[0] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL);
//        if(getline(ifs, line))
//            bfieldTmp[1] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL);
//        if(getline(ifs, line))
//            bfieldTmp[2] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL);
//        bField.setField(bfieldTmp);
//        if(getline(ifs, line))
//            nParticles      = atoi(line.substr(line.find(" ") + 1).c_str());
//        if(getline(ifs, line))
//        {
//            double tmp        = strtod(line.substr(line.find(" ") + 1).c_str(), NULL);
//            z0Range = {{-tmp, tmp}};
//        }
//        if(getline(ifs, line))
//            phiRange[0]       = strtod(line.substr(line.find(" ") + 1).c_str(), NULL);
//        if(getline(ifs, line))
//            phiRange[1]       = strtod(line.substr(line.find(" ") + 1).c_str(), NULL);
//        if(getline(ifs, line))
//            d0Range[0]       = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_m;
//        if(getline(ifs, line))
//            d0Range[1]       = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_m;
//        if(getline(ifs, line))
//            etaRange[0]       = strtod(line.substr(line.find(" ") + 1).c_str(), NULL);
//        if(getline(ifs, line))
//            etaRange[1]       = strtod(line.substr(line.find(" ") + 1).c_str(), NULL);
//        if(getline(ifs, line))
//            ptRange[0]       = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_MeV;
//        if(getline(ifs, line))
//            ptRange[1]       = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_MeV;
//        if(getline(ifs, line))
//            mass = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_MeV;
//        if(getline(ifs, line))
//            charge = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_e;
//        if(getline(ifs, line))
//            pID = atoi(line.substr(line.find(" ") + 1).c_str()); 
//
//        halfX = nStrips * pitch / 2;
//        halfY = lengthStrip + sensorGap / 2; 
//        thickness        = 2 * thicknessSCT + thicknessSupport;
//
//        ifs.close();
//    }
//
//    void printConfig()
//    {
//        std::cout << "thicknessSCT: " << thicknessSCT << std::endl;  
//        std::cout << "thicknessSupport: " << thicknessSupport << std::endl;
//        std::cout << "thickness: " << thickness << std::endl;
//        std::cout << "X0: " << X0 << std::endl;
//        std::cout << "L0: " << L0 << std::endl;
//        std::cout << "A: " << A << std::endl;
//        std::cout << "Z: " << Z << std::endl;
//        std::cout << "Rho: " << Rho << std::endl;
//        std::cout << "planeZ: " << planeZ[0] << " "
//                                  << planeZ[1] << " "
//                                  << planeZ[2] << " "
//                                  << planeZ[3] << " "
//                                  << planeZ[4] << " "
//                                  << planeZ[5] << " "
//                                  << planeZ[6] << " "
//                                  << planeZ[7] << " "
//                                  << planeZ[8] << " "
//                                  << planeZ[9] << " "
//                                  << planeZ[10] << " "
//                                  << planeZ[11] << std::endl;
//        std::cout << "posFirstSur: " << posFirstSur << std::endl;
//        std::cout << "eps: " << eps << std::endl;
//        std::cout << "lorentzAngle: " << lorentzAngle << std::endl;
//        std::cout << "sensorGap: " << sensorGap << std::endl;
//        std::cout << "nStrips: " << nStrips << std::endl;
//        std::cout << "pitch: " << pitch << std::endl;
//        std::cout << "lengthStrip: " << lengthStrip << std::endl;
//        std::cout << "halfX: " << halfX << std::endl;
//        std::cout << "halfY: " << halfY << std::endl;
//        std::cout << "rotation: " << rotation << std::endl << std::endl;
//        std::cout << "nEvents: " << nEvents << std::endl;
//        Acts::Vector3D dummyVec(0., 0., 0.);
//        std::cout << "bField: " << bField.getField(dummyVec)[0] << " " << bField.getField(dummyVec)[1] << " " << bField.getField(dummyVec)[2] << std::endl;
//        std::cout << "nParticles: " << nParticles << std::endl;
//        std::cout << "z0Range: " << z0Range[0] << " " << z0Range[1] << std::endl;
//        std::cout << "d0Range: " << d0Range[0] << " " << d0Range[1] << std::endl;
//        std::cout << "phiRange: " << phiRange[0] << " " << phiRange[1] << std::endl;
//        std::cout << "etaRange: " << etaRange[0] << " " << etaRange[1] << std::endl;
//        std::cout << "ptRange: " << ptRange[0] << " " << ptRange[1] << std::endl;
//        std::cout << "mass: " << mass << std::endl;
//        std::cout << "charge: " << charge << std::endl;
//        std::cout << "pID: " << pID << std::endl;
//    }
//};
//

}

