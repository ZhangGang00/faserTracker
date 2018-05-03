#include "FaserTracker/common_includes.hh"
#include "FaserTracker/Settings.hh"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


namespace FaserTracker {

    Settings::Settings(const std::string & settingsFile) {
        boost::property_tree::ptree properties;
        read_json(settingsFile, properties);
        cout << "INFO  Loaded settings file: " << settingsFile << "\n";

        // debug
        debug.chain  = properties.get("debug.chain", false);
        debug.hits   = properties.get("debug.hits", false);
        debug.tracks = properties.get("debug.tracks", false);

        // input chain
        inputChain.name = properties.get("inputChain.name", "hits");

        // events
        events.eventNumberStart = properties.get("events.eventNumberStart", -1);
        events.eventNumberEnd   = properties.get("events.eventNumberEnd", -1);

        // tracks
        tracks.trackIdStart = properties.get("tracks.trackIdStart", -1);
        tracks.trackIdEnd   = properties.get("tracks.trackIdEnd", -1);
        tracks.countTracks  = properties.get("tracks.countTracks", false);
        tracks.fitTracks    = properties.get("tracks.fitTracks", false);
        tracks.plotHits     = properties.get("tracks.plotHits", false);
        tracks.plotDigits   = properties.get("tracks.plotDigits", false);
        tracks.plotTruth    = properties.get("tracks.plotTruth", false);
        tracks.compareTruth = properties.get("tracks.compareTruth", false);

        // clustering
        digitClusters.distanceTolerance = properties.get("digitClusters.distanceTolerance", 0.01);
        digitClusters.dumpClusters      = properties.get("digitClusters.dumpClusters", false);

        // track finding

        // track fitting


        cout << "INFO  Using the following settings:\n"
             << "\n"
             << "        debug.chain  = " << debug.chain  << "\n"
             << "        debug.hits   = " << debug.hits   << "\n"
             << "        debug.tracks = " << debug.tracks << "\n"
             << "\n"
             << "        inputChain.name = " << inputChain.name << "\n"
             << "\n"
             << "        events.eventNumberStart = " << events.eventNumberStart << "\n"
             << "        events.eventNumberEnd   = " << events.eventNumberEnd << "\n"
             << "\n"
             << "        tracks.trackIdStart = " << tracks.trackIdStart << "\n"
             << "        tracks.trackIdEnd   = " << tracks.trackIdEnd << "\n"
             << "        tracks.countTracks  = " << tracks.countTracks << "\n"
             << "        tracks.fitTracks    = " << tracks.fitTracks << "\n"
             << "        tracks.plotHits     = " << tracks.plotHits << "\n"
             << "        tracks.plotDigits   = " << tracks.plotDigits << "\n"
             << "        tracks.plotTruth    = " << tracks.plotTruth << "\n"
             << "        tracks.compareTruth = " << tracks.compareTruth << "\n"
             << "\n"
             << "        digitClusters.distanceTolerance = " << digitClusters.distanceTolerance << "\n"
             << "        digitClusters.dumpClusters      = " << digitClusters.dumpClusters << "\n"
             //<< "\n"
             //<< "        trackFinding.blah = " << trackFinding.blah << "\n"
             //<< "\n"
             //<< "        trackFitting.blah = " << trackFitting.blah << "\n"
             << "\n";
    }

}

