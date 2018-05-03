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
            std::string name = "faser";
        } inputChain;

        struct {
            int   eventNumberStart = -1;
            int   eventNumberEnd   = -1;
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

        struct {
            double distanceTolerance = 0.01;
            bool   dumpClusters      = false;
        } digitClusters;

        struct {
        } trackFinding;

        struct {
        } trackFitting;
    
        Settings(const std::string & settingsFile);
    };
}

