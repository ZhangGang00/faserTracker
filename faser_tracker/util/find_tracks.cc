// adapted from Geant4 example

#include "FaserTracker/DigiReader.hh"
#include "FaserTracker/HitInfo.hh"
//#include "FaserTracker/TestTrackFit.hh"
#include "FaserTracker/Plots.hh"
#include "FaserTracker/DigiClusterFinder.hh"
#include "FaserTracker/DigiCluster.hh"
#include "FaserTracker/TrackCandidate.hh"
#include "FaserTracker/TrackFinder.hh"
#include "FaserTracker/TrackFitter.hh"
#include "FaserTracker/common_includes.hh"

#include "FaserTrackerGeometry.hh"
#include "FaserTrackerEvent.hh"
#include "FaserTrackerSpacePoint.hh"
//#include "FaserSensorHit.hh"

#include <cstdlib>

#include "TROOT.h"
#include "TSystem.h"
#include "TChain.h"
#include "TFile.h"
#include "TGraph2D.h"
#include "TGraph.h"


void usage(string executable) {
    cout << "Usage: " << executable << " [options]" << "\n"
         << "Options:\n"
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

    const char * _faserDir = std::getenv("FASER_TRACKER_DIR");
    if (!_faserDir) {
        cout << "ERROR  The environment variable `FASER_TRACKER_DIR`\n"
             << "       must be set to the name of the directory\n"
             << "       containing the `faser_tracker` package.\n";
        return 1;
    }
    string faserDir = _faserDir;

    // Load input space points & geometry settings
    //cout << "Loading shared library...\n";
    //gSystem->Load("libFaserTrackerClassesDict");
    string loadCommand = ".L " + faserDir + "/faser_tracker/external/faserMC/include/FaserTrackerClasses.hh+";
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


    //FaserTracker::DigiReader digiReader {*eventTree};
    //cout << "INFO  DigiReader initialized with input chain.\n";

    //int eventCounter = 0;
    //map<int, int> trackIdCounter;

    //shared_ptr<TFile> outputFile;
    //shared_ptr<FaserTracker::Plots> plots;
    //if (settings->tracks.plotTruth) {
    //    outputFile = make_shared<TFile>("truth_track_info.root", "recreate");
    //    plots = make_shared<FaserTracker::Plots>(settings);
    //}


    //for (long iEvent = 0; iEvent < eventTree->GetEntries(); ++iEvent) {
    //    if (settings->events.eventNumberStart > -1 && iEvent < settings->events.eventNumberStart) continue;
    //    if (settings->events.eventNumberEnd   > -1 && iEvent > settings->events.eventNumberEnd) break;

    //    digiReader.getEntry(iEvent);

    //    if (settings->debug.chain || (iEvent>0 && iEvent%1000==0)) {
    //        cout << "INFO  Checking chain entry " << iEvent
    //             << "   nDigits = " << digiReader.digiPlane.size()
    //             << "   nTruth  = " << digiReader.truthPlane.size() << "\n";
    //    }

    //    ++eventCounter;
    //    if (nEntries > -1 && eventCounter > nEntries) break;

    //    if (settings->tracks.countTracks) {
    //        for (int trackId : digiReader.truthTrack) {
    //            if (settings->tracks.truthIdStart > -1 && trackId < settings->tracks.truthIdStart) continue;
    //            if (settings->tracks.truthIdEnd   > -1 && trackId > settings->tracks.truthIdEnd) continue;

    //            if (trackIdCounter.count(trackId) < 1) trackIdCounter[trackId] = 0;
    //            ++trackIdCounter[trackId];
    //        }
    //    }

    //    if (settings->tracks.plotTruth) {
    //        plots->plotTruthDigits(digiReader, *outputFile, iEvent);
    //    }


    //    if (settings->digitClusters.dumpClusters ||
    //        settings->digitClusters.dumpTruthTrackIds ||
    //        settings->trackFinding.findClusterTracks)
    //    {
    //        FaserTracker::DigiClusterFinder dcf {settings->digitClusters.distanceTolerance};
    //        shared_ptr<vector<FaserTracker::DigiCluster>> digiClusters = dcf.findDigitClusters(digiReader);

    //        if (settings->digitClusters.dumpClusters) {
    //            cout << "INFO  Dumping digit clusters\n\n";

    //            for (const FaserTracker::DigiCluster & cluster : *digiClusters) {
    //                cluster.print();
    //            }
    //            cout << "\n";
    //        }

    //        if (settings->digitClusters.dumpTruthTrackIds) {
    //            cout << "INFO  Dumping truth track IDs for digits in clusters\n\n";

    //            for (const FaserTracker::DigiCluster & cluster : *digiClusters) {
    //                cluster.printTruthTrackIds();
    //            }
    //            cout << "\n";
    //        }


    //        if (settings->trackFinding.findClusterTracks) {

    //            cout << "INFO  Finding cluster tracks\n";

    //            FaserTracker::TrackFinder finder {settings};
    //            vector<FaserTracker::TrackCandidate> candidates = finder.findLinearYZ(*digiClusters);

    //            if (settings->trackFinding.saveTracks) {
    //                cout << "INFO  Saving cluster tracks found\n";

    //                for (const FaserTracker::TrackCandidate & candidate : candidates) {
    //                    finder.saveTrack(candidate);
    //                }
    //            }

    //            if (settings->trackFitting.fitAndSaveTracks) {
    //                cout << "INFO  Fitting and saving cluster tracks\n";

    //                FaserTracker::TrackFitter fitter {settings};

    //                for (const FaserTracker::TrackCandidate & candidate : candidates) {
    //                    fitter.fitAndSaveCircularXZ(candidate);
    //                }
    //            }

    //        }

    //    }

    //    if (settings->trackFinding.findTruthTracks &&
    //        settings->trackFitting.fitAndSaveTracks)
    //    {
    //        cout << "INFO  Finding truth tracks\n";

    //        FaserTracker::TrackFinder finder {settings};
    //        vector<FaserTracker::TrackCandidate> candidates = finder.findTruthTracks(*digiReader.digits());

    //        if (settings->trackFinding.saveTracks) {
    //            cout << "INFO  Saving truth tracks found\n";

    //            for (const FaserTracker::TrackCandidate & candidate : candidates) {
    //                finder.saveTrack(candidate);
    //            }
    //        }

    //        cout << "INFO  Fitting and saving truth tracks\n";
    //        FaserTracker::TrackFitter fitter {settings};

    //        for (const FaserTracker::TrackCandidate & candidate : candidates) {
    //            fitter.fitAndSaveCircularXZ(candidate);
    //        }
    //    }


    //}

    //// Note: This is counting the number of tracks across events.
    ////       Set `eventNumberMin` = `eventNumberMax` to count the number of tracks
    ////       in a single event.
    //if (settings->tracks.countTracks) {
    //    cout << "INFO  Dumping digit counts for track IDs in range specified by\n"
    //         << "      settings.tracks.truthIdStart, settings.tracks.truthIdEnd:\n"
    //         << "\n";

    //    // `trackIdCounter` internally sorted by key as it was populated so no need to sort
    //    for (const auto & t : trackIdCounter) {
    //        cout << "        Track ID " << t.first << "\t" << t.second << "\n";
    //    }
    //    cout << "\n";
    //}

}

