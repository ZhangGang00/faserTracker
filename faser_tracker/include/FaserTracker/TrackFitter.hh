//#pragma once
//
//#include "FaserTracker/Settings.hh"
//#include "FaserTracker/TrackCandidate.hh"
//#include "TF1.h"
//#include <string>
//
//
//namespace FaserTracker {
//
//    class TrackFitter {
//
//        struct Impl;
//        std::unique_ptr<Impl> impl;
//
//    public:
//
//        enum {NONE, GLOBAL, KALMAN} strategy;
//
//        TrackFitter(std::shared_ptr<Settings> settings) {
//            setFitMethod(settings->trackFitting.strategy);
//        }
//
//        setStrategy(std::string & strategy_);
//
//
//        FittedTrack fitTrackCandidate(const TrackCandidate & candidate);
//
//    };
//
//}
//
