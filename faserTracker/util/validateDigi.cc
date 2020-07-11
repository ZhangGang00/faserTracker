#include "FaserTracker/TrackCandidate.hh"
#include "FaserTracker/TrackFinder.hh"
#include "FaserTracker/TrackFitter.hh"
#include "FaserTracker/common_includes.hh"

#include "FaserTrackerGeometry.hh"
#include "FaserTrackerEvent.hh"
#include "FaserTrackerSpacePoint.hh"
#include "FaserTrackerDigit.hh"
#include "FaserTrackerCluster.hh"
#include "FaserTrackerTruthHit.hh"
#include "FaserTrackerTruthParticle.hh"

#include <cstdlib>

#include "TChain.h"
#include "TFile.h"
#include "TGraph.h"


//------------------------------------------------------------------------------

void usage(string executable) {
  cout << "Usage: " << executable << " [options]" << '\n'
       << "Options:\n"
       << "  -h/--help                      print help\n"
       << "  -i/--input FILE_NAME           path to input ROOT file\n"
       << "  -n/--nEntries N_ENTRIES        number of entries from input to process\n"
       << '\n';
}

//------------------------------------------------------------------------------

int main(int argc, char ** argv) {
  // Command-line options
  string  input = "";
  long    nEntries = -1;
  bool    verbose = false;

  int iOpt = 1;
  while (iOpt < argc) {
    string sw = argv[iOpt];
    if      (sw=="-h" || sw=="--help"      ) { usage(argv[0]); return 0; }
    else if (sw=="-i" || sw=="--input"     ) { input = argv[++iOpt]; }
    else if (sw=="-n" || sw=="--nEntries"  ) { nEntries = stoi(argv[++iOpt]); }
    else if (sw=="-v" || sw=="--verbose"   ) { verbose = true; }
    else {
      cout << "Unknown switch: " << sw << '\n';
      usage(argv[0]);
      return 1;
    }
    ++iOpt;
  }
  cout << "Flags:\n"
    << "------\n"
    << "  nEntries   = " << nEntries   << '\n'
    << "  input      = " << input      << '\n'
    << "  verbose    = " << verbose    << '\n'
    << '\n';


  // Load input space points & geometry settings
  // TODO: Put this in a helper class instead of writing this for each and every executable.

  TFile inputFile {input.c_str()};

  TTree * eventTree = (TTree*) inputFile.Get("events");
  if (eventTree == nullptr) {
    cout << "ERROR  Unable to load event tree: `events`\n"
      << "       Exiting.\n"
      << '\n';
    return 1;
  }
  cout << "INFO  Loaded event tree `events` from "
    << input << " with " << eventTree->GetEntries() << " entries.\n";

  TTree * geoTree = (TTree*) inputFile.Get("geo");
  if (geoTree == nullptr) {
    cout << "ERROR  Unable to load event tree: `geo`\n"
      << "       Exiting.\n"
      << '\n';
    return 1;
  }
  long nGeoEntries = geoTree->GetEntries();
  cout << "INFO  Loaded input geometry tree `geo` from "
    << input << " with " << nGeoEntries << " entries.\n";
  if (nGeoEntries != 1) {
    cout << "ERROR  The geometry tree `geo` has " << nGeoEntries << " entries.\n"
      << "       It should have exactly one entry. Exiting.\n"
      << '\n';
    return 1;
  }

  FaserTrackerGeometry * geo = nullptr;
  geoTree->SetBranchAddress("geo", &geo);
  geoTree->GetEntry(0);

  FaserTrackerEvent * event = nullptr;
  eventTree->SetBranchAddress("events", &event);


  // Analyze digits, analog/digital clusters, & space points

  TFile outputFile {"validateDigi.root", "recreate"};

  TGraph digitVsHit [3];
  TGraph aclVsHit [3];
  TGraph dclVsHit [3];
  TGraph aclVsDigit [3];
  TGraph dclVsDigit [3];
  TGraph spVsHit [3];
  TGraph spVsCl [3];

  for (int iEntry=0; iEntry<eventTree->GetEntries(); ++iEntry) {
  //for (int iEntry=0; iEntry<500; ++iEntry) {
    eventTree->GetEntry(iEntry);
    cout << "INFO  Processing event " << event->eventNumber << '\n';

    for (const FaserTrackerSpacePoint * sp : event->spacePoints) {
      double spPos [3] = {sp->globalPos.X(), sp->globalPos.Y(), sp->globalPos.Z()};
      if (verbose) {
        cout << "INFO    SpacePoint(" << sp->plane << ',' << sp->module << ',' << sp->sensor << ',' << sp->row << ',' << sp->charge
                                      << ",(" << spPos[0] << ',' << spPos[1] << ',' << spPos[2] << "))\n";
      }
/*
      for (const FaserTrackerCluster * cl : sp->analogClusters) {
        double clPos [3] = {cl->globalPos.X(), cl->globalPos.Y(), cl->globalPos.Z()};
        if (verbose) {
          cout << "INFO      AnalogCluster(" << cl->plane << ',' << cl->module << ',' << cl->sensor << ',' << cl->row << ',' << cl->weightedStrip << ',' << cl->charge
                                             << ",(" << clPos[0] << ',' << clPos[1] << ',' << clPos[2] << "))\n";
        }
        for (int i = 0; i < 3; ++i) {
          spVsCl[i].SetPoint(spVsCl[i].GetN(), clPos[i], spPos[i]);
        }

        for (const FaserTrackerDigit * digit : cl->digits) {
          double digitPos [3] = {digit->globalPos.X(), digit->globalPos.Y(), digit->globalPos.Z()};
          if (verbose) {
            cout << "INFO        Digit(" << digit->plane << ',' << digit->module << ',' << digit->sensor << ',' << digit->row << ',' << digit->strip << ',' << digit->charge
                                         << ",(" << digitPos[0] << ',' << digitPos[1] << ',' << digitPos[2] << "))\n";
          }
          for (int i = 0; i < 3; ++i) {
            aclVsDigit[i].SetPoint(aclVsDigit[i].GetN(), digitPos[i], clPos[i]);
            dclVsDigit[i].SetPoint(dclVsDigit[i].GetN(), digitPos[i], clPos[i]);
          }

          for (uint iHit : digit->truthHitIndices) {
            FaserTrackerTruthHit * hit = event->truthHits[iHit];
            double hitPos [3] = {hit->globalPos.X(), hit->globalPos.Y(), hit->globalPos.Z()};
            for (int i = 0; i < 3; ++i) {
              digitVsHit[i].SetPoint(digitVsHit[i].GetN(), hitPos[i], digitPos[i]);
              spVsHit[i].SetPoint(spVsHit[i].GetN(), hitPos[i], spPos[i]);
            }

          }
        }
      }
 */
                   for (uint icl : sp->analogClusterIndices) {
		      FaserTrackerCluster * cl = event->analogClusters[icl];
                      for (uint idigit : cl->digitIndices) {
			 FaserTrackerDigit * digit = event->digits[idigit];
                         for (uint iHit : digit->truthHitIndices) { 
				   cout<<"truthHitIndices="<<iHit<<"\n";
			 }
		      }
		   }
    }

    if (verbose) {
      for (const FaserTrackerTruthHit * hit : event->truthHits) {
        double hitPos [3] = {hit->globalPos.X(), hit->globalPos.Y(), hit->globalPos.Z()};
        cout << "INFO    TruthHit(" << hit->plane << ',' << hit->module << ',' << hit->sensor << ',' << hit->row << ',' << hit->strip << ',' << hit->energy
                                  << ",(" << hitPos[0] << ',' << hitPos[1] << ',' << hitPos[2] << "))\n";
      }
    }
  }

  for (uint i = 0; i < 3; ++i) {
    string component;
    switch (i) {
    case 0:
      component = "x";
      break;
    case 1:
      component = "y";
      break;
    case 2:
      component = "z";
      break;
    }

    outputFile.mkdir("digitVsHit");
    outputFile.cd("digitVsHit");
    string name = "digitVsHit_" + component;
    digitVsHit[i].Write(name.c_str());

    outputFile.mkdir("analogClusterVsHit");
    outputFile.cd("analogClusterVsHit");
    name = "analogClusterVsHit_" + component;
    aclVsHit[i].Write(name.c_str());

    outputFile.mkdir("digitalClusterVsHit");
    outputFile.cd("digitalClusterVsHit");
    name = "digitalClusterVsHit_" + component;
    aclVsHit[i].Write(name.c_str());

    outputFile.mkdir("clusterVsDigit");
    outputFile.cd("clusterVsDigit");
    name = "clusterVsDigit_" + component;
    aclVsDigit[i].Write(name.c_str());

    outputFile.mkdir("spacePointVsHit");
    outputFile.cd("spacePointVsHit");
    name = "spacePointVsHit_" + component;
    spVsHit[i].Write(name.c_str());

    outputFile.mkdir("spacePointVsCluster");
    outputFile.cd("spacePointVsCluster");
    name = "spacePointVsCluster_" + component;
    spVsCl[i].Write(name.c_str());
  }

  return 0;
}

