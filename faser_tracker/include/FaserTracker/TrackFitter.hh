#pragma once

#include "FaserTracker/Settings.hh"
#include "FaserTracker/TrackCandidate.hh"
#include "FaserTracker/FittedTrack.hh"
#include <memory>


//------------------------------------------------------------------------------

namespace FaserTracker {

    class TrackFitter {

        struct Impl;
        std::unique_ptr<Impl> impl;

    public:

        enum {GLOBAL, KALMAN} strategy;

        TrackFitter(std::shared_ptr<Settings> settings);

        FittedTrack fitTrackCandidate(TrackCandidate & candidate);

    };

}

