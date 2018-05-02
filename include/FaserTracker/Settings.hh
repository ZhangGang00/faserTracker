#pragma once

#include <string>

namespace FaserTracker {

    struct Settings {

        struct {
            bool  chain  = false;
            bool  hits   = false;
            bool  tracks = false;
        } debug;

        struct {
            int   eventNumberStart = -1;
            int   eventNumberEnd = -1;
        } events;

        struct {
            int   trackIdStart = 2;
            int   trackIdEnd   = 2;
            bool  countTracks  = false;
            bool  fitTracks    = false;
            bool  plotHits     = false;
            bool  plotDigits   = false;
            bool  plotTruth    = false;
            bool  compareTruth = false;
        } tracks;
    
        Settings(const std::string & settingsFile);
    };
}

