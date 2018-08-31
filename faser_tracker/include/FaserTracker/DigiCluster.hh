#pragma once

#include "FaserTracker/DigiCluster.hh"
#include "FaserTracker/Digit.hh"
#include <memory>
#include <vector>

class TVector3;


namespace FaserTracker {

    /**
     * A `DigiCluster` is a collection of nearby digits in the same FASER plane.
     */
    struct DigiCluster {

        long    eventNumber;
        int     plane;
        double  charge;
        int     truthTrackId;

        std::shared_ptr<std::vector<Digit>> digits;

        DigiCluster(int plane_) :
            eventNumber{-1},
            plane {plane_},
            charge {0},
            truthTrackId {-1},
            digits {std::make_shared<std::vector<Digit>>()}
        {
        }

        void addDigit(const Digit & digit, bool isTruth=false);

        std::shared_ptr<TVector3> globalPosition() const;

        void print() const;
        void printTruthTrackIds() const;

    private:

        mutable std::shared_ptr<TVector3> _cachedGlobalPosition;

    };

}

