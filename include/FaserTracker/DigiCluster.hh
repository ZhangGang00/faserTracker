#pragma once

#include <memory>
#include <vector>
#include "FaserTracker/Digit.hh"

class TVector3;


namespace FaserTracker {

    /**
     * A `DigiCluster` is a collection of nearby digits in the same FASER plane.
     */
    struct DigiCluster {

        int plane;
        std::shared_ptr<std::vector<Digit>> digits;

        DigiCluster(int plane_) :
            plane {plane_}
        {
            digits = std::make_shared<std::vector<Digit>>();
        }

        std::shared_ptr<TVector3> globalPosition() const;

    private:

        mutable std::shared_ptr<TVector3> _cachedGlobalPosition;

    };

}

