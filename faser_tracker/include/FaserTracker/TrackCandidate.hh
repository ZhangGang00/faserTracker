#pragma once

#include <string>
#include <vector>
#include "FaserTrackerSpacePoint.hh"


namespace FaserTracker {

    struct TrackCandidate {

        using SpacePoint = FaserTrackerSpacePoint;

        //TODO: Read this in from geometry output instead of hardcoding.
        static constexpr int N_PLANES = 8;

        long eventNumber;
        std::vector<SpacePoint> spacePoints;
        std::string label;
        int truthTrackId;


        TrackCandidate() :
            eventNumber {-1},
            label {""},
            truthTrackId {-1}
        {
        }

        void addSpacePoint(const SpacePoint & sp);

        bool isValid() const;

        void dumpSpacePoints() const;

    };

}

