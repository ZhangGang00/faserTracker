#include "FaserTracker/common_includes.hh"
#include "FaserTracker/DigiCluster.hh"


namespace FaserTracker {

    shared_ptr<TVector3> DigiCluster::globalPosition() const {

        if (_cachedGlobalPosition != nullptr) {
            return _cachedGlobalPosition;
        }

        double totalCharge = 0;
        auto centerOfCharge = make_shared<TVector3>();
        for (const Digit & d : *digits) {
            totalCharge += d.charge;
            *centerOfCharge += d.charge * *d.globalPosition();
        }
        *centerOfCharge *= 1.0/totalCharge;

        _cachedGlobalPosition = centerOfCharge;
        return centerOfCharge;

    }

}

