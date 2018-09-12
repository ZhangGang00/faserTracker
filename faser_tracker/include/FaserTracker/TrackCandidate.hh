#pragma once

#include "FaserTrackerSpacePoint.hh"
#include "FaserTrackerGeometry.hh"

#include <memory>
#include <string>
#include <vector>


namespace FaserTracker {

    using SpacePoint = FaserTrackerSpacePoint;


    struct TrackCandidate {

        long eventNumber;
        std::shared_ptr<std::vector<SpacePoint>> spacePoints;
        int truthTrackId;


        TrackCandidate() :
            eventNumber {-1},
            spacePoints {std::make_shared<std::vector<SpacePoint>>()},
            truthTrackId {-1}
        {
        }

        //void addCluster(const DigiCluster & cluster);

        bool isValid() const;

        //void dumpDigitClusters() const;

    };

}

