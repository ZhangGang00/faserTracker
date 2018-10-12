//#pragma once
//
//#include "FaserTrackerSpacePoint.hh"
//#include "FaserTracker/Settings.hh"
//#include "FaserTracker/TrackCandidate.hh"
//#include <memory>
//#include <vector>
//
//
//namespace FaserTracker {
//
//    struct TrackFinder {
//
//        using SpacePoint = FaserTrackerSpacePoint;
//
//        std::shared_ptr<Settings> settings;
//        double chargeThreshold = 0;
//        double yTolerance = 0;
//
//        std::vector<SpacePoint> spacePoints;
//
//        TrackFinder(std::shared_ptr<Settings> settings_) :
//            settings {settings_},
//            chargeThreshold {settings->trackFinding.chargeThreshold},
//            yTolerance {settings->trackFinding.yTolerance}
//        {
//        }
//
//        std::vector<TrackCandidate> findLinearYZ(const std::vector<SpacePoint> & sp);
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
