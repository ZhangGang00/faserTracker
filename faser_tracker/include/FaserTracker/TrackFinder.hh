//#pragma once
//
//#include <vector>
//#include "FaserTracker/Settings.hh"
//#include "FaserTracker/DigiReader.hh"
//#include "FaserTracker/Digit.hh"
//#include "FaserTracker/DigiCluster.hh"
//#include "FaserTracker/TrackCandidate.hh"
//
//
//namespace FaserTracker {
//
//    struct TrackFinder {
//
//        std::shared_ptr<Settings> settings;
//        double chargeThreshold = 0;
//        double yTolerance = 0;
//
//        //TODO: read in this info from faserMC/faserGeo.mac
//        static constexpr int N_PLANES    = 5;
//        static constexpr int START_PLANE = 0;
//        static constexpr int END_PLANE   = N_PLANES - 1;
//
//        //std::vector<DigitCluster> digitClusters;
//
//        TrackFinder(std::shared_ptr<Settings> settings_) :
//            settings {settings_},
//            chargeThreshold {settings->trackFinding.chargeThreshold},
//            yTolerance {settings->trackFinding.yTolerance}
//        {
//        }
//
//        std::vector<TrackCandidate> findTruthTracks(const std::vector<Digit> & digits);
//
//        std::vector<TrackCandidate> findLinearYZ(const std::vector<Digit> & digits);
//        std::vector<TrackCandidate> findLinearYZ(const std::vector<DigiCluster> & digiClusters);
//
//        void saveTrack(const TrackCandidate & candidate);
//
//
//    private:
//
//        long _nTracksFound = 0;
//
//    };
//
//}
//
