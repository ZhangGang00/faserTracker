// adapted from Geant4 example

#include <cstdlib>
#include "common_includes.hh"
#include "TChain.h"
#include "FaserSensorHit.hh"
#include "G4ThreeVector.hh"
#include "FaserTracker/HitInfo.hh"
#include "FaserTracker/Debugger.hh"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

//#include "ACTS/Detector/TrackingGeometry.hpp"
//#include "ACTS/EventData/Measurement.hpp"
//#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
//#include "ACTS/Extrapolation/ExtrapolationEngine.hpp"
//#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"
//#include "ACTS/Extrapolation/MaterialEffectsEngine.hpp"
//#include "ACTS/Extrapolation/RungeKuttaEngine.hpp"
//#include "ACTS/Extrapolation/StaticEngine.hpp"
//#include "ACTS/Extrapolation/StaticNavigationEngine.hpp"
//#include "ACTS/Fitter/KalmanFitter.hpp"
//#include "ACTS/Fitter/KalmanUpdator.hpp"
//#include "ACTS/MagneticField/ConstantBField.hpp"
//#include "ACTS/Surfaces/PerigeeSurface.hpp"
//#include "ACTS/Utilities/Definitions.hpp"
//#include "ACTS/Utilities/Logger.hpp"

void usage(string executable) {
    cout << "Usage: " << executable << " [options]" << "\n"
         << "Options:\n"
         << "  -c/--checkEvent EVENT_NUMBER   event number on which to dump full info\n"
         << "  -d/--debug DEBUG_LEVEL         debug level 0-3 (0: none, 3: max)\n"
         << "  -h/--help                      print help\n"
         << "  -i/--input FILE_NAME           path to input ROOT file\n"
         << "  -n/--nEntries N_ENTRIES        number of entries from input to process\n"
         << "\n";
}

int main(int argc, char ** argv) {
    string input = "";
    long   nEntries = -1;
    long   checkEvent = -1;

    int iOpt = 1;
    while (iOpt < argc) {
        string sw = argv[iOpt];
        if      (sw=="-c" || sw=="--checkEvent") { checkEvent = stoi(argv[++iOpt]); }
        else if (sw=="-h" || sw=="--help"      ) { usage(argv[0]); return 0; }
        else if (sw=="-i" || sw=="--input"     ) { input = argv[++iOpt]; }
        else if (sw=="-n" || sw=="--nEntries"  ) { nEntries = stoi(argv[++iOpt]); }
        else {
            cout << "Unknown switch: " << sw << "\n";
            usage(argv[0]);
            return 1;
        }
        ++iOpt;
    }
    cout << "Flags:\n"
         << "------\n"
         << "  checkEvent = " << checkEvent << "\n"
         << "  nEntries   = " << nEntries   << "\n"
         << "  input      = " << input      << "\n"
         << "\n";

    const char * faserDir = std::getenv("FASER_BASE_DIR");
    if (!faserDir) {
        cout << "ERROR  The environment variable `FASER_BASE_DIR`\n"
             << "       must be set to the name of the directory\n"
             << "       containing the `faser_tracker` package.\n";
        return 1;
    }
    string propertyFile = string(faserDir) + "/faser_tracker/settings.json";
    boost::property_tree::ptree properties;
    read_json(propertyFile, properties);
    cout << "INFO  Loaded settings file: " << propertyFile << "\n";
    FaserTracker::Debugger debug;
    debug.chain = properties.get("debug.chain", 0);
    debug.hits  = properties.get("debug.hits", 0);
    cout << "INFO  Using the following properties:\n"
         << "        debug.chain = " << debug.chain << "\n"
         << "        debug.hits  = " << debug.hits  << "\n"
         << "\n";

    //TODO:
    // 1. Set up ACTS surfaces
    // 2. Feed global hit positions to ACTS surface machinery
    // 3. Find tracks
    // 4. Fit tracks
    vector<FaserSensorHit*> hits;
    vector<FaserSensorHit*> * pHits = &hits;

    auto inputChain = make_shared<TChain>("faser");
    inputChain->Add(input.c_str());
    inputChain->SetBranchAddress("Hits", &pHits);
    cout << "INFO  Loaded input " << input << " with " << inputChain->GetEntries() << " entries.\n";;

    for (long i = 0; i < inputChain->GetEntries(); ++i) {
        if (debug.chain > 1 || (i>0 && i%1000==0)) {
            cout << "INFO  Checking chain entry " << i << "   nHits = " << hits.size() << "\n";
        }
        inputChain->GetEntry(i);

        for (const FaserSensorHit * hit : hits) {
            if (debug.hits > 1 || (debug.hits > 0 && checkEvent==i)) {
                FaserTracker::dumpHitInfo(*hit);
            }

            //G4ThreeVector globalPosition = hit->GetGlobalPos();
            //G4ThreeVector localPosition = hit->GetLocalPos();
        }
    }

    //shared_ptr<const Acts::TrackingGeometry> geo = Acts::buildGenericDetector(Acts::Logging::VERBOSE,
    //        Acts::Logging::VERBOSE,
    //        Acts::Logging::VERBOSE,
    //        0);
    //ActsVector<ParValue_t, NGlobalPars> pars;
    //pars << 0, 0, M_PI / 2, M_PI / 2, 0.0001;
    //auto startCov = std::make_unique<ActsSymMatrix<ParValue_t, NGlobalPars>>(
    //        ActsSymMatrix<ParValue_t, NGlobalPars>::Identity());

    //const Surface* pSurf   = geo->getBeamline();
    //auto           startTP = std::make_unique<BoundParameters>(
    //        std::move(startCov), std::move(pars), *pSurf);

    //ExtrapolationCell<TrackParameters> exCell(*startTP);
    //exCell.addConfigurationMode(ExtrapolationMode::CollectSensitive);
    //exCell.addConfigurationMode(ExtrapolationMode::StopAtBoundary);

    //auto exEngine = initExtrapolator(geo);
    //exEngine->extrapolate(exCell);

    //std::cout << "got " << exCell.extrapolationSteps.size()
    //    << " extrapolation steps" << std::endl;

    //std::vector<FitMeas_t> vMeasurements;
    //vMeasurements.reserve(exCell.extrapolationSteps.size());

    //// identifier
    //long int id = 0;
    //// random numbers for smearing measurements
    //std::default_random_engine             e;
    //std::uniform_real_distribution<double> std_loc1(1, 5);
    //std::uniform_real_distribution<double> std_loc2(0.1, 2);
    //std::normal_distribution<double>       g(0, 1);

    //double std1, std2, l1, l2;
    //for (const auto& step : exCell.extrapolationSteps) {
    //    const auto& tp = step.parameters;
    //    if (tp->associatedSurface().type() != Surface::Cylinder) continue;

    //    std1 = std_loc1(e);
    //    std2 = std_loc2(e);
    //    l1   = tp->get<eLOC_1>() + std1 * g(e);
    //    l2   = tp->get<eLOC_2>() + std2 * g(e);
    //    ActsSymMatrixD<2> cov;
    //    cov << std1 * std1, 0, 0, std2 * std2;
    //    vMeasurements.push_back(Meas_t<eLOC_1, eLOC_2>(
    //                tp->associatedSurface(), id, std::move(cov), l1, l2));
    //    ++id;
    //}

    //std::cout << "created " << vMeasurements.size() << " pseudo-measurements"
    //    << std::endl;
    //for (const auto& m : vMeasurements) std::cout << m << std::endl << std::endl;

    //KalmanFitter<MyExtrapolator, CacheGenerator, NoCalibration, GainMatrixUpdator>
    //    KF;
    //KF.m_oCacheGenerator = CacheGenerator();
    //KF.m_oCalibrator     = NoCalibration();
    //KF.m_oExtrapolator   = MyExtrapolator(exEngine);
    //KF.m_oUpdator        = GainMatrixUpdator();

    //std::cout << "start fit" << std::endl;
    //auto track = KF.fit(vMeasurements, std::make_unique<BoundParameters>(*startTP));

    //// dump track
    //for (const auto & p : track) {
    //    std::cout << *p->getCalibratedMeasurement() << std::endl;
    //    std::cout << *p->getSmoothedState() << std::endl;
    //}

    // Values available in a `FaserSensorHit`:
    //G4int GetTrackID() const           { return fTrackID; };
    //G4int GetPlaneID() const           { return fPlaneID; };
    //G4int GetModuleID() const          { return fModuleID; };
    //G4int GetSensorID() const          { return fSensorID; };
    //G4int GetRowID() const             { return fRowID; };
    //G4int GetStripID() const           { return fStripID; };
    //G4double GetEdep() const           { return fEdep; };
    //G4ThreeVector GetGlobalPos() const { return fGlobalPos; };
    //G4ThreeVector GetLocalPos() const  { return fLocalPos; };
}

