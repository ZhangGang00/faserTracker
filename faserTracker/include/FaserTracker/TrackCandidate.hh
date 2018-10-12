#pragma once

#include <string>
#include <vector>
#include "FaserTrackerSpacePoint.hh"
#include "TLorentzVector.h"


namespace FaserTracker {

    struct TrackCandidate {

        using SpacePoint = FaserTrackerSpacePoint;

        //TODO: Read this in from geometry output instead of hardcoding.
        static constexpr int N_PLANES = 8;

        long eventNumber;
        std::vector<SpacePoint> spacePoints;
        std::string label;
        int truthTrackId;

        double parameters [5]; // rho, theta, phi, x, y
        TLorentzVector fourMomentum;
        TVector3 vertex;
        //TODO: add truth info here


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

