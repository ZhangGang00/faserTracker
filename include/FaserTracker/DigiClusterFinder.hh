#pragma once

#include <memory>
#include <vector>
#include "FaserTracker/DigiReader.hh"
#include "FaserTracker/DigiCluster.hh"


namespace FaserTracker {

    struct DigiClusterFinder {

        // TODO: Read this in from faserMC/faserGeo.mac instead of hardcoding here
        static constexpr int N_PLANES = 5;

        double distanceTolerance;

        // TODO: Also incorporate charge deposited


        DigiClusterFinder(double distanceTolerance_) :
            distanceTolerance {distanceTolerance_}
        {
        }
    
        std::shared_ptr<std::vector<DigiCluster>>
            findDigitClusters(const DigiReader & digiReader);
    
    };

}

