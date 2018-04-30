// adapted from Geant4 example

#include <cstdlib>
#include "common_includes.hh"
#include "TChain.h"
#include "TFile.h"
#include "TGraph2D.h"
#include "TGraph.h"
//#include "FaserSensorHit.hh"
#include "FaserTracker/DigiReader.hh"
#include "G4ThreeVector.hh"
#include "FaserTracker/HitInfo.hh"
#include "FaserTracker/Debugger.hh"
#include "FaserTracker/TrackInfo.hh"
#include "FaserTracker/TrackFit.hh"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


void usage(string executable) {
    cout << "Usage: " << executable << " [options]" << "\n"
         << "Options:\n"
         << "  -d/--debug DEBUG_LEVEL         debug level 0-3 (0: none, 3: max)\n"
         << "  -h/--help                      print help\n"
         << "  -i/--input FILE_NAME           path to input ROOT file\n"
         << "  -n/--nEntries N_ENTRIES        number of entries from input to process\n"
         << "\n";
}

int main(int argc, char ** argv) {
    string input = "";
    long   nEntries = -1;

    int iOpt = 1;
    while (iOpt < argc) {
        string sw = argv[iOpt];
        if      (sw=="-h" || sw=="--help"      ) { usage(argv[0]); return 0; }
        else if (sw=="-i" || sw=="--input"     ) { input = argv[++iOpt]; }
        else if (sw=="-n" || sw=="--nEntries"  ) { nEntries = stoi(argv[++iOpt]); }
        else {
            cout << "Unknown switch: " << sw << "\n";
            usage(argv[0]);
            return 1;
        }
        ++iOpt;
    }
    cout << "Flags:\n"
         << "------\n"
         << "  nEntries   = " << nEntries   << "\n"
         << "  input      = " << input      << "\n"
         << "\n";

    const char * faserDir = std::getenv("FASER_BASE_DIR");
    if (!faserDir) {
        cout << "ERROR  The environment variable `FASER_BASE_DIR`\n"
             << "       must be set to the name of the directory\n"
             << "       containing the `faser_tracker` package.\n";
        return 1;
    }
    string propertyFile = string(faserDir) + "/faser_tracker_run/settings.json";
    boost::property_tree::ptree properties;
    read_json(propertyFile, properties);
    cout << "INFO  Loaded settings file: " << propertyFile << "\n";
    FaserTracker::Debugger debug;
    debug.chain  = properties.get("debug.chain", 0);
    debug.hits   = properties.get("debug.hits", 0);
    debug.tracks = properties.get("debug.tracks", 0);
    int nEventMin = properties.get("events.nEventMin", -1);
    int nEventMax = properties.get("events.nEventMax", -1);
    FaserTracker::TrackInfo trackInfo;
    trackInfo.plotTracks = properties.get("tracks.plotTracks", 0);
    trackInfo.trackId    = properties.get("tracks.trackId", -1);
    cout << "INFO  Using the following properties:\n"
         << "        debug.chain          = " << debug.chain  << "\n"
         << "        debug.hits           = " << debug.hits   << "\n"
         << "        debug.tracks         = " << debug.tracks << "\n"
         << "        events.nEventMin     = " << nEventMin << "\n"
         << "        events.nEventMax     = " << nEventMax << "\n"
         << "        trackInfo.plotTracks = " << trackInfo.plotTracks << "\n"
         << "        trackInfo.trackId    = " << trackInfo.trackId << "\n"
         << "\n";

    auto inputChain = make_shared<TChain>("hits");
    inputChain->Add(input.c_str());
    cout << "INFO  Loaded input " << input << " with " << inputChain->GetEntries() << " entries.\n";;

    FaserTracker::DigiReader digiReader {*inputChain};
    cout << "INFO  DigiReader initialized with input chain.\n";

    int eventCounter = 0;
    int trackIdCounter [50] = {0};
    TFile outputFile {"track_info.root", "recreate"};
    TGraph2D trackPoints;
    TGraph   trackPointsZX;
    TGraph   trackPointsZY;
    for (long i = 0; i < inputChain->GetEntries(); ++i) {
        if (nEventMin > -1 && i < nEventMin) continue;
        if (nEventMax > -1 && i > nEventMax) continue;

        inputChain->GetEntry(i);
        if (debug.chain > 1 || (i>0 && i%1000==0)) {
            cout << "INFO  Checking chain entry " << i
                 << "   nDigits = " << digiReader.digiPlane.size()
                 << "   nTruth  = " << digiReader.truthPlane.size() << "\n";
        }

        ++eventCounter;
        if (nEntries > -1 && eventCounter > nEntries) break;
        for (int j = 0; j < digiReader.truthGlobalX.size(); ++j) {
            //if (debug.hits > 1 || debug.hits > 0) {
            //    FaserTracker::dumpHitInfo(*hit);
            //}
            int trackId = digiReader.truthTrack[j];
            if (trackId >= 0 && trackId < 50) ++trackIdCounter[trackId];

            if (trackInfo.plotTracks > 0 && trackInfo.trackId == trackId) {
                double x = digiReader.truthGlobalX[j];
                double y = digiReader.truthGlobalY[j];
                double z = digiReader.truthGlobalZ[j];
                trackPoints.SetPoint(trackPoints.GetN(), x, y, z);
                trackPointsZX.SetPoint(trackPointsZX.GetN(), z, x);
                trackPointsZY.SetPoint(trackPointsZY.GetN(), z, y);
            }

            //G4ThreeVector globalPosition = hit->GetGlobalPos();
            //G4ThreeVector localPosition = hit->GetLocalPos();
        }
    }
    if (debug.tracks > 0) {
        for (int i = 0; i < 50; ++i) {
            cout << "Track ID " << i << "\t" << trackIdCounter[i] << "\n";
        }
    }
    outputFile.cd();
    if (trackInfo.plotTracks > 0) {
        cout << "INFO  Saving TGraph2D `trackPoints` with " << trackPoints.GetN()
             << " entries to output file track_info.root\n";
        trackPoints.Write("trackPoints");
        cout << "INFO  Saving TGraph `trackPointsZX` with " << trackPointsZX.GetN()
             << " entries to output file track_info.root\n";
        trackPointsZX.SetMarkerStyle(3);
        trackPointsZX.Write("trackPointsZX");
        cout << "INFO  Saving TGraph `trackPointsZY` with " << trackPointsZY.GetN()
             << " entries to output file track_info.root\n";
        trackPointsZY.SetMarkerStyle(3);
        trackPointsZY.Write("trackPointsZY");
    }
    FaserTracker::TrackFit::fitCircle(trackPointsZX);

    // Values available in a `FaserSensorHit`:
    //G4int GetTrackID() const           { return fTrackID; };
    //G4int GetPlaneID() const           { return fPlaneID; };
    //G4int GetModuleID() const          { return fModuleID; };
    //G4int GetSensorID() const          { return fSensorID; };
    //G4int GetRowID() const             { return fRowID; };
    //G4int GetStripID() const           { return fStripID; };
    //G4double GetEdep() const           { return fEdep; };
    //G4ThreeVector GetGlobalPos() const { return fGlobalPos; };
    //G4ThreeVector GetLocalPos() const  { return fLocalPos; };
}

