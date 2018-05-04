#pragma once

#include <string>

namespace FaserTracker {

    struct Settings {

        struct {
            bool  chain  = false;
            bool  hits   = false;
            bool  digits = false;
            bool  tracks = false;
        } debug;

        struct {
            std::string  name = "faser";
        } inputChain;

        struct {
            int  eventNumberStart = -1;
            int  eventNumberEnd   = -1;
        } events;

        struct {
            int   truthIdStart = 2;
            int   truthIdEnd   = 2;
            bool  countTracks  = false;
            bool  fitTracks    = false;
            bool  plotHits     = false;
            bool  plotDigits   = false;
            bool  plotTruth    = false;
        } tracks;

        struct {
            double distanceTolerance = 0.2;
            bool   dumpClusters      = false;
            bool   dumpTruthTrackIds = false;
        } digitClusters;

        struct {
            double  chargeThreshold   = 0.5;
            double  yTolerance        = 0.5;
            bool    findTruthTracks   = false;
            bool    findClusterTracks = false;
            bool    saveTracks        = false;
        } trackFinding;

        struct {
            bool  fitAndSaveTracks = false;
        } trackFitting;
    
        Settings(const std::string & settingsFile);
    };
}

