// adapted from Geant4 example

#include "FaserTracker/TrackCandidate.hh"
#include "FaserTracker/TrackFinder.hh"
#include "FaserTracker/TrackFitter.hh"
#include "FaserTracker/common_includes.hh"

#include "FaserTrackerGeometry.hh"
#include "FaserTrackerEvent.hh"
#include "FaserTrackerSpacePoint.hh"

#include <cstdlib>

#include "TROOT.h"
#include "TSystem.h"
#include "TChain.h"
#include "TFile.h"
#include "TGraph2D.h"
#include "TGraph.h"


//------------------------------------------------------------------------------

void usage(string executable) {
    cout << "Usage: " << executable << " [options]" << "\n"
         << "Options:\n"
         << "  -h/--help                      print help\n"
         << "  -i/--input FILE_NAME           path to input ROOT file\n"
         << "  -n/--nEntries N_ENTRIES        number of entries from input to process\n"
         << "\n";
}

//------------------------------------------------------------------------------

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

    const char * _faserDir = std::getenv("FASER_TRACKER_DIR");
    if (!_faserDir) {
        cout << "ERROR  The environment variable `FASER_TRACKER_DIR`\n"
             << "       must be set to the name of the directory\n"
             << "       containing the `faser_tracker` package.\n";
        return 1;
    }
    string faserDir = _faserDir;

    // Load input space points & geometry settings
    string loadCommand = ".L " + faserDir + "/faser_tracker/external/faserMC/faserMC/include/FaserTrackerClasses.hh+";
    gROOT->ProcessLine(loadCommand.c_str());

    // Load settings
    string settingsFile = string(faserDir) + "/faser_tracker_run/settings.json";
    auto settings = make_shared<FaserTracker::Settings>(settingsFile);

    TFile inputFile {input.c_str()};

    TTree * eventTree = (TTree*) inputFile.Get(settings->input.eventTreeName.c_str());
    if (eventTree == nullptr) {
        cout << "ERROR  Unable to load event tree: " << settings->input.eventTreeName << "\n"
             << "       Exiting.\n"
             << "\n";
        return 1;
    }
    cout << "INFO  Loaded event tree " << settings->input.eventTreeName << " from "
         << input << " with " << eventTree->GetEntries() << " entries.\n";

    TTree * geoTree = (TTree*) inputFile.Get(settings->input.geometryTreeName.c_str());
    if (geoTree == nullptr) {
        cout << "ERROR  Unable to load event tree: " << settings->input.geometryTreeName << "\n"
             << "       Exiting.\n"
             << "\n";
        return 1;
    }
    long nGeoEntries = geoTree->GetEntries();
    cout << "INFO  Loaded input geometry tree " << settings->input.geometryTreeName << " from "
         << input << " with " << nGeoEntries << " entries.\n";
    if (nGeoEntries != 1) {
        cout << "ERROR  The geometry tree " << geoTree->GetName() << " has " << nGeoEntries << " entries.\n"
             << "       It should have exactly one entry. Exiting.\n"
             << "\n";
        return 1;
    }

    FaserTrackerGeometry * geo = nullptr;
    geoTree->SetBranchAddress("geo", &geo);
    geoTree->GetEntry(0);

    FaserTrackerEvent * event = nullptr;
    eventTree->SetBranchAddress("events", &event);
    for (int iEntry=0; iEntry<eventTree->GetEntries(); ++iEntry) {
        eventTree->GetEntry(iEntry);
        cout << "INFO  Processing event " << event->eventNumber << "\n";
        for (const FaserTrackerSpacePoint * sp : event->spacePoints) {
            const TVector3 & pos = sp->globalPos;
            cout << "      Read space point with global position (" << pos.X() << "," << pos.Y() << "," << pos.Z() << ")\n";
        }
    }

}

