//#pragma once
//
//#include "FaserTracker/Settings.hh"
//#include "FaserTracker/TrackCandidate.hh"
//#include <string>
//#include <vector>
//
//
//namespace FaserTracker {
//
//    class TrackFinder {
//
//        struct Impl;
//        std::unique_ptr<Impl> impl;
//
//    public:
//
//        enum {NONE, GLOBAL, SEED} strategy;
//
//        double yTolerance = 0;
//
//        std::vector<SpacePoint> spacePoints;
//
//        TrackFinder(std::shared_ptr<Settings> settings)
//            : impl {std::make_unique<Impl>()}
//            , yTolerance {settings->trackFinding.yTolerance}
//        {
//            setStrategy(settings->trackFinding.strategy);
//        }
//
//        void setStrategy(const std::string & strategy);
//
//        //std::vector<TrackCandidate> findTruthTracks(const std::vector<Digit> & digits);
//
//        //std::vector<TrackCandidate> findLinearYZ(const std::vector<Digit> & digits);
//        //std::vector<TrackCandidate> findLinearYZ(const std::vector<DigiCluster> & digiClusters);
//
//        //void saveTrack(const TrackCandidate & candidate);
//
//    };
//
//}
//
