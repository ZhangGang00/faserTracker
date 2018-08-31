#pragma once

#include "FaserTracker/Settings.hh"
#include "FaserTracker/TrackCandidate.hh"
#include "TF1.h"


namespace FaserTracker {

    struct TrackFitter {

        //TODO: read in this info from faserMC/faserGeo.mac
        static constexpr int N_PLANES    = 5;
        static constexpr int START_PLANE = 0;
        static constexpr int END_PLANE   = N_PLANES - 1;


        TrackFitter(std::shared_ptr<Settings> settings)
        {
        }

        void fitAndSaveCircularXZ(const TrackCandidate & candidate);
    };

}

