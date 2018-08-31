#pragma once

#include <string>
#include <vector>
#include "FaserTracker/DigiCluster.hh"


namespace FaserTracker {

    /**
     * A `DigiCluster` is a collection of nearby digits in the same FASER plane.
     */
    struct TrackCandidate {

        long eventNumber;
        std::shared_ptr<std::vector<DigiCluster>> digiClusters;
        std::string label;
        int truthTrackId;

        //TODO: read in this info from faserMC/faserGeo.mac
        static constexpr int N_PLANES    = 5;
        static constexpr int START_PLANE = 0;
        static constexpr int END_PLANE   = N_PLANES - 1;


        TrackCandidate() :
            eventNumber {-1},
            digiClusters {std::make_shared<std::vector<DigiCluster>>()},
            label {""},
            truthTrackId {-1}
        {
        }

        void addCluster(const DigiCluster & cluster);

        bool isValid() const;

        void dumpDigitClusters() const;

    };

}

