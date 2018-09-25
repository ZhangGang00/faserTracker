#pragma once

#include "FaserTracker/Settings.hh"
#include "Acts/EventData/TrackParameters.hpp"
#include <vector>


namespace FaserTracker {

    struct FittedTrack {

        bool  isValid;

        //Acts::BoundParameters  params;


        FittedTrack()
            : isValid {false}
        {
        }

    };

}

