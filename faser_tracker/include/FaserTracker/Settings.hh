#pragma once

#include <string>

namespace FaserTracker {

    struct Settings {

        struct {
            std::string  fileName         = "FaserMCEvent_tracking.root";
            std::string  eventTreeName    = "events";
            std::string  geometryTreeName = "geo";
        } input;

        struct {
            std::string  fileName = "faser_tracker_out.root";
        } output;

        struct {
            bool  input       = false;
            bool  spacePoints = false;
            bool  tracks      = false;
        } debug;

        struct {
            int  eventNumberStart = -1;
            int  eventNumberEnd   = -1;
        } events;

        struct {
            int   truthIdStart = 2;
            int   truthIdEnd   = 2;
        } tracks;

        struct {
            std::string  method     = "none";
        } trackFinding;

        struct {
            std::string  method     = "none";
        } trackFitting;
    
        Settings(const std::string & settingsFile);
    };
}

