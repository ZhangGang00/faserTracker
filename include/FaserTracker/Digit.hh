#pragma once

#include "TVector3.h"


namespace FaserTracker {

    struct Digit {

        // TODO: Implement `Digit`s using only (plane, module, sensor, row, strip) combo
        int     plane;
        //int     module;
        //int     sensor;
        //int     row;
        //int     strip;
        double  charge;

        // Truth info (for "cheating")
        TVector3  truthGlobalPos;
        int       truthTrackId;

        Digit(int plane_, double charge_, TVector3 truthGlobalPos_, int truthTrackId_) :
            plane {plane_},
            charge {charge_},
            truthGlobalPos {truthGlobalPos_},
            truthTrackId {truthTrackId_}
        {
        }
        //Digit(int plane, int module, int sensor, int row, int strip) :
        //    plane {plane},
        //    module {module},
        //    sensor {sensor},
        //    row {row},
        //    strip {strip}
        //{
        //}

        // TODO: Implement this to using only (plane, module, sensor, row, strip) combo
        std::shared_ptr<TVector3> globalPosition() const;


    private:

        mutable std::shared_ptr<TVector3> _cachedGlobalPosition;

    };

}

