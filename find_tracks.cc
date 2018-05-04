// adapted from Geant4 example

#include <cstdlib>
#include "FaserTracker/common_includes.hh"
#include "TChain.h"
#include "TFile.h"
#include "TGraph2D.h"
#include "TGraph.h"
//#include "FaserSensorHit.hh"
#include "FaserTracker/DigiReader.hh"
#include "G4ThreeVector.hh"
#include "FaserTracker/HitInfo.hh"
//#include "FaserTracker/TestTrackFit.hh"
#include "FaserTracker/Plots.hh"
#include "FaserTracker/DigiClusterFinder.hh"
#include "FaserTracker/DigiCluster.hh"
#include "FaserTracker/TrackCandidate.hh"
#include "FaserTracker/TrackFinder.hh"
#include "FaserTracker/TrackFitter.hh"


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

    const char * faserDir = std::getenv("FASER_BASE_DIR");
    if (!faserDir) {
        cout << "ERROR  The environment variable `FASER_BASE_DIR`\n"
             << "       must be set to the name of the directory\n"
             << "       containing the `faser_tracker` package.\n";
        return 1;
    }

    // Load settings
    string settingsFile = string(faserDir) + "/faser_tracker_run/settings.json";
    auto settings = make_shared<FaserTracker::Settings>(settingsFile);

    auto inputChain = make_shared<TChain>(settings->inputChain.name.c_str());
    inputChain->Add(input.c_str());
    cout << "INFO  Loaded input " << input << " with " << inputChain->GetEntries() << " entries.\n";;

    FaserTracker::DigiReader digiReader {*inputChain};
    cout << "INFO  DigiReader initialized with input chain.\n";

    int eventCounter = 0;
    map<int, int> trackIdCounter;

    shared_ptr<TFile> outputFile;
    shared_ptr<FaserTracker::Plots> plots;
    if (settings->tracks.plotTruth) {
        outputFile = make_shared<TFile>("truth_track_info.root", "recreate");
        plots = make_shared<FaserTracker::Plots>(settings);
    }


    for (long iEvent = 0; iEvent < inputChain->GetEntries(); ++iEvent) {
        if (settings->events.eventNumberStart > -1 && iEvent < settings->events.eventNumberStart) continue;
        if (settings->events.eventNumberEnd   > -1 && iEvent > settings->events.eventNumberEnd) break;

        digiReader.getEntry(iEvent);

        if (settings->debug.chain || (iEvent>0 && iEvent%1000==0)) {
            cout << "INFO  Checking chain entry " << iEvent
                 << "   nDigits = " << digiReader.digiPlane.size()
                 << "   nTruth  = " << digiReader.truthPlane.size() << "\n";
        }

        ++eventCounter;
        if (nEntries > -1 && eventCounter > nEntries) break;

        if (settings->tracks.countTracks) {
            for (int trackId : digiReader.truthTrack) {
                if (settings->tracks.truthIdStart > -1 && trackId < settings->tracks.truthIdStart) continue;
                if (settings->tracks.truthIdEnd   > -1 && trackId > settings->tracks.truthIdEnd) continue;

                if (trackIdCounter.count(trackId) < 1) trackIdCounter[trackId] = 0;
                ++trackIdCounter[trackId];
            }
        }

        if (settings->tracks.plotTruth) {
            plots->plotTruthDigits(digiReader, *outputFile, iEvent);
        }


        if (settings->digitClusters.dumpClusters ||
            settings->trackFinding.findClusterTracks)
        {
            FaserTracker::DigiClusterFinder dcf {settings->digitClusters.distanceTolerance};
            shared_ptr<vector<FaserTracker::DigiCluster>> digiClusters = dcf.findDigitClusters(digiReader);

            if (settings->digitClusters.dumpClusters) {

                cout << "INFO  Dumping digit clusters\n\n";

                for (const FaserTracker::DigiCluster & cluster : *digiClusters) {
                    cluster.print();
                }
                cout << "\n";

            }


            if (settings->trackFinding.findClusterTracks) {

                cout << "INFO  Finding cluster tracks\n";

                FaserTracker::TrackFinder finder {settings};
                vector<FaserTracker::TrackCandidate> candidates = finder.findLinearYZ(*digiClusters);

                if (settings->trackFinding.saveTracks) {
                    cout << "INFO  Saving cluster tracks found\n";

                    for (const FaserTracker::TrackCandidate & candidate : candidates) {
                        finder.saveTrack(candidate);
                    }
                }

                if (settings->trackFitting.fitAndSaveTracks) {
                    cout << "INFO  Fitting and saving cluster tracks\n";

                    FaserTracker::TrackFitter fitter {settings};

                    for (const FaserTracker::TrackCandidate & candidate : candidates) {
                        fitter.fitAndSaveCircularXZ(candidate);
                    }
                }

            }

        }

        if (settings->trackFinding.findTruthTracks &&
            settings->trackFitting.fitAndSaveTracks)
        {
            cout << "INFO  Finding truth tracks\n";

            FaserTracker::TrackFinder finder {settings};
            vector<FaserTracker::TrackCandidate> candidates = finder.findTruthTracks(*digiReader.digits());

            if (settings->trackFinding.saveTracks) {
                cout << "INFO  Saving truth tracks found\n";

                for (const FaserTracker::TrackCandidate & candidate : candidates) {
                    finder.saveTrack(candidate);
                }
            }

            cout << "INFO  Fitting and saving truth tracks\n";
            FaserTracker::TrackFitter fitter {settings};

            for (const FaserTracker::TrackCandidate & candidate : candidates) {
                fitter.fitAndSaveCircularXZ(candidate);
            }
        }


    }

    // Note: This is counting the number of tracks across events.
    //       Set `eventNumberMin` = `eventNumberMax` to count the number of tracks
    //       in a single event.
    if (settings->tracks.countTracks) {
        cout << "INFO  Dumping digit counts for track IDs in range specified by\n"
             << "      settings.tracks.truthIdStart, settings.tracks.truthIdEnd:\n"
             << "\n";

        // `trackIdCounter` internally sorted by key as it was populated so no need to sort
        for (const auto & t : trackIdCounter) {
            cout << "        Track ID " << t.first << "\t" << t.second << "\n";
        }
        cout << "\n";
    }


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

