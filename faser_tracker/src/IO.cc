#include "FaserTracker/IO.hh"
#include "FaserTracker/Settings.hh"
#include "FaserTracker/common_includes.hh"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"


namespace FaserTracker {

//------------------------------------------------------------------------------

IO::IO()
    //: settings {make_shared<Settings>(faserDir() + "/faser_tracker_run/settings.json")}
{
    settings = make_shared<Settings>(faserDir() + "/faser_tracker_run/settings.json");
}

IO::IO(const string & settingsFile) {

    settings = make_shared<Settings>(settingsFile);

    loadFaserTrackerClasses();
    inputFile = unique_ptr<TFile>();
    outputFile = unique_ptr<TFile>();

    //inputFile = make_unique<TFile>();

    //eventTree = static_cast<TTree*>(inputFile.Get(settings->plotSpacePoints.input.eventTreeName.c_str()));
    //if (!eventTree) {
    //    cout << "ERROR  Unable to load event tree: " << settings->input.eventTreeName << '\n'
    //         << "       Exiting.\n"
    //         << '\n';
    //    return 1;
    //}
    //cout << "INFO  Loaded event tree " << settings->input.eventTreeName << " from "
    //     << input << " with " << eventTree->GetEntries() << " entries.\n";

    //geometryTree = (TTree*) inputFile.Get(settings->input.geometryTreeName.c_str());
    //if (!geometryTree) {
    //    cout << "ERROR  Unable to load event tree: " << settings->input.geometryTreeName << '\n'
    //         << "       Exiting.\n"
    //         << '\n';
    //    return 1;
    //}
    //long nGeoEntries = geometryTree->GetEntries();
    //cout << "INFO  Loaded input geometry tree " << settings->input.geometryTreeName << " from "
    //     << input << " with " << nGeoEntries << " entries.\n";
    //if (nGeoEntries != 1) {
    //    cout << "ERROR  The geometry tree " << geometryTree->GetName() << " has " << nGeoEntries << " entries.\n"
    //         << "       It should have exactly one entry. Exiting.\n"
    //         << '\n';
    //    return 1;
    //}

    //FaserTrackerGeometry * geo = nullptr;
    //geometryTree->SetBranchAddress("geo", &geo);
    //geometryTree->GetEntry(0);

    //FaserTrackerEvent * event = nullptr;
    //eventTree->SetBranchAddress("events", &event);
}

//------------------------------------------------------------------------------

std::string IO::faserDir() {
    const char * _faserDir = std::getenv("FASER_TRACKER_DIR");

    if (!_faserDir) {
        cout << "ERROR  The environment variable `FASER_TRACKER_DIR`\n"
             << "       must be set to the name of the directory\n"
             << "       containing the `faser_tracker` package.\n";
        throw runtime_error {"IO::faserDir: `FASER_TRACKER_DIR` environment variable not set"};
    }

    return string {_faserDir};
}

//------------------------------------------------------------------------------

void IO::loadFaserTrackerClasses() {
    string loadCommand = ".L " + faserDir() + "/faser_tracker/external/faserMC/include/FaserTrackerClasses.hh+";
    gROOT->ProcessLine(loadCommand.c_str());
}

//------------------------------------------------------------------------------

TTree * IO::geometryTree() {
    if (geoTree) return geoTree;
    if (inputFile) return static_cast<TTree*>(inputFile->Get(settings->input.geometryTreeName.c_str()));
    return nullptr;
}

//------------------------------------------------------------------------------

TTree * IO::eventTree() {
    if (evtTree) return evtTree;
    if (inputFile) return static_cast<TTree*>(inputFile->Get(settings->input.eventTreeName.c_str()));
    return nullptr;
}

//------------------------------------------------------------------------------

IO::~IO() = default;

//------------------------------------------------------------------------------

}

