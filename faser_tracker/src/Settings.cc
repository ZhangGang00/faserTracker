#include "FaserTracker/common_includes.hh"
#include "FaserTracker/Settings.hh"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


namespace FaserTracker {

    Settings::Settings(const std::string & settingsFile) {
        boost::property_tree::ptree properties;
        read_json(settingsFile, properties);
        cout << "INFO  Loaded settings file: " << settingsFile << '\n';

        // input
        input.fileName         = properties.get("input.fileName", "FaserMCEvent_tracking.root");
        input.eventTreeName    = properties.get("input.eventTreeName", "events");
        input.geometryTreeName = properties.get("input.geometryTreeName", "geo");

        // output
        output.fileName = properties.get("output.fileName", "faser_tracker_out.root");

        // debug
        debug.input       = properties.get("debug.input", false);
        debug.spacePoints = properties.get("debug.spacePoints", false);
        debug.tracks      = properties.get("debug.tracks", false);

        // events
        events.eventNumberStart = properties.get("events.eventNumberStart", -1);
        events.eventNumberEnd   = properties.get("events.eventNumberEnd", -1);

        // tracks
        tracks.truthIdStart = properties.get("tracks.truthIdStart", -1);
        tracks.truthIdEnd   = properties.get("tracks.truthIdEnd", -1);

        // track finding
        trackFinding.method          = properties.get("trackFinding.method", "truth");
        trackFinding.chargeThreshold = properties.get("trackFinding.chargeThreshold", 0.5);
        trackFinding.yTolerance      = properties.get("trackFinding.yTolerance", 0.5);

        // track fitting
        trackFitting.method = properties.get("trackFitting.method", "global");


        cout << "INFO  Using the following settings:\n"
             << '\n'
             << "        debug.input       = " << debug.input << '\n'
             << "        debug.spacePoints = " << debug.spacePoints << '\n'
             << "        debug.tracks      = " << debug.tracks << '\n'
             << '\n'
             << "        input.fileName        = " << input.fileName << '\n'
             << "        input.eventTreeName   = " << input.eventTreeName << '\n'
             << "        input.geomtryTreeName = " << input.geometryTreeName << '\n'
             << '\n'
             << "        output.fileName = " << output.fileName << '\n'
             << '\n'
             << "        events.eventNumberStart = " << events.eventNumberStart << '\n'
             << "        events.eventNumberEnd   = " << events.eventNumberEnd << '\n'
             << '\n'
             << "        tracks.truthIdStart = " << tracks.truthIdStart << '\n'
             << "        tracks.truthIdEnd   = " << tracks.truthIdEnd << '\n'
             << '\n'
             << "        trackFinding.method          = " << trackFinding.method << '\n'
             << "        trackFinding.chargeThreshold = " << trackFinding.chargeThreshold << '\n'
             << "        trackFinding.yTolerance      = " << trackFinding.yTolerance << '\n'
             << '\n'
             << "        trackFitting.method = " << trackFitting.method << '\n'
             << '\n';
    }

}

