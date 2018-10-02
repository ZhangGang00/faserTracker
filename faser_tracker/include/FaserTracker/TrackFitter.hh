#pragma once

#include "FaserTracker/Settings.hh"
#include "FaserTracker/TrackCandidate.hh"
#include "FaserTrackerSpacePoint.hh"
#include "TF1.h"


namespace FaserTracker {

    struct TrackFitter {

        using SpacePoint = FaserTrackerSpacePoint;

        TrackFitter(std::shared_ptr<Settings> settings)
        {
        }

        void fitAndSaveCircularXZ(const TrackCandidate & candidate);
    };

}

