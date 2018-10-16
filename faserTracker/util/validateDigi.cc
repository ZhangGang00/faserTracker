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

#include "TSystem.h"
#include "TChain.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"


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

  // Plane/module/sensor/row selection (-1 for all)
  int  plane  = -1;
  int  module = -1;
  int  sensor = -1;
  int  row    = -1;

  int iOpt = 1;
  while (iOpt < argc) {
    string sw = argv[iOpt];
    if      (sw=="-h" || sw=="--help"      ) { usage(argv[0]); return 0; }
    else if (sw=="-i" || sw=="--input"     ) { input = argv[++iOpt]; }
    else if (sw=="-n" || sw=="--nEntries"  ) { nEntries = stoi(argv[++iOpt]); }
    else if (sw=="-v" || sw=="--verbose"   ) { verbose = true; }
    else if (sw=="-p" || sw=="--plane"     ) { plane  = stoi(argv[++iOpt]); }
    else if (sw=="-m" || sw=="--module"    ) { module = stoi(argv[++iOpt]); }
    else if (sw=="-s" || sw=="--sensor"    ) { sensor = stoi(argv[++iOpt]); }
    else if (sw=="-r" || sw=="--row"       ) { row    = stoi(argv[++iOpt]); }
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

  //TFile outputFile {"validateDigi.root", "recreate"};

  TGraph digitVsHit [3];
  TGraph aclVsHit [3];
  TGraph dclVsHit [3];
  TGraph aclVsDigit [3];
  TGraph dclVsDigit [3];
  TGraph spVsHit [3];
  TGraph spVsCl [3];

  for (int iEntry=0; iEntry<eventTree->GetEntries(); ++iEntry) {
    eventTree->GetEntry(iEntry);
    cout << "INFO  Processing event " << event->eventNumber << '\n';

    for (const FaserTrackerSpacePoint * sp : event->spacePoints) {
      if (plane  >= 0 && sp->plane  != plane ) continue;
      if (module >= 0 && sp->module != module) continue;
      if (sensor >= 0 && sp->sensor != sensor) continue;
      if (row    >= 0 && sp->row    != row   ) continue;

      double spPos [3] = {sp->globalPos.X(), sp->globalPos.Y(), sp->globalPos.Z()};
      if (verbose) {
        cout << "INFO    SpacePoint(" << sp->plane << ',' << sp->module << ',' << sp->sensor << ',' << sp->row << ',' << sp->charge
                                      << ",(" << spPos[0] << ',' << spPos[1] << ',' << spPos[2] << "))\n";
      }

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
    }

    if (verbose) {
      for (const FaserTrackerTruthHit * hit : event->truthHits) {
        double hitPos [3] = {hit->globalPos.X(), hit->globalPos.Y(), hit->globalPos.Z()};
        cout << "INFO    TruthHit(" << hit->plane << ',' << hit->module << ',' << hit->sensor << ',' << hit->row << ',' << hit->strip << ',' << hit->energy
                                  << ",(" << hitPos[0] << ',' << hitPos[1] << ',' << hitPos[2] << "))\n";
      }
    }
  }


  string outputDir = "validateDigi_";
  if (plane >= 0) outputDir += to_string(plane);
  else outputDir += 'A'; // 'A' for all
  if (module >= 0) outputDir += to_string(module);
  else outputDir += 'A'; // 'A' for all
  if (sensor >= 0) outputDir += to_string(sensor);
  else outputDir += 'A'; // 'A' for all
  if (row >= 0) outputDir += to_string(row);
  else outputDir += 'A'; // 'A' for all

  cout << "INFO  Creating directory `" << outputDir << "` and dumping output there...\n";
  gSystem->mkdir(outputDir.c_str());
  gSystem->cd(outputDir.c_str());

  TCanvas canvas;
  canvas.cd();

  for (uint i = 0; i < 3; ++i) {
    string component;
    double axisMin = -100.;;
    double axisMax =  100.;

    switch (i) {
    case 0:
      component = "x";
      axisMin = -100.;
      axisMax =  100.;
      break;
    case 1:
      component = "y";
      axisMin = -100.;
      axisMax =  100.;
      break;
    case 2:
      component = "z";
      axisMin = -100.;
      axisMax = 2100.;
      break;
    }

    if (digitVsHit[i].GetN() > 0) {
      canvas.Clear();
      //outputFile.mkdir("digitVsHit");
      //outputFile.cd("digitVsHit");
      //string name = "digitVsHit_" + component;
      //digitVsHit[i].Write(name.c_str());
      string name = "digitVsHit_" + component + ".png";
      digitVsHit[i].GetXaxis()->SetTitle((component+"Hit [mm]").c_str());
      digitVsHit[i].GetYaxis()->SetTitle((component+"Digit [mm]").c_str());
      digitVsHit[i].GetXaxis()->SetRangeUser(axisMin, axisMax);
      digitVsHit[i].GetYaxis()->SetRangeUser(axisMin, axisMax);
      digitVsHit[i].Draw("ap*");
      canvas.SaveAs(name.c_str());
    }

    if (aclVsHit[i].GetN() > 0) {
      canvas.Clear();
      //outputFile.mkdir("analogClusterVsHit");
      //outputFile.cd("analogClusterVsHit");
      string name = "analogClusterVsHit_" + component + ".png";
      aclVsHit[i].GetXaxis()->SetTitle((component+"Hit [mm]").c_str());
      aclVsHit[i].GetYaxis()->SetTitle((component+"AnalogCluster [mm]").c_str());
      aclVsHit[i].GetXaxis()->SetRangeUser(axisMin, axisMax);
      aclVsHit[i].GetYaxis()->SetRangeUser(axisMin, axisMax);
      aclVsHit[i].Draw("ap*");
      canvas.SaveAs(name.c_str());
    }

    if (aclVsHit[i].GetN() > 0) {
      canvas.Clear();
      //outputFile.mkdir("digitalClusterVsHit");
      //outputFile.cd("digitalClusterVsHit");
      string name = "digitalClusterVsHit_" + component + ".png";
      aclVsHit[i].GetXaxis()->SetTitle((component+"Hit [mm]").c_str());
      aclVsHit[i].GetYaxis()->SetTitle((component+"DigitalCluster [mm]").c_str());
      aclVsHit[i].GetXaxis()->SetRangeUser(axisMin, axisMax);
      aclVsHit[i].GetYaxis()->SetRangeUser(axisMin, axisMax);
      aclVsHit[i].Draw("ap*");
      canvas.SaveAs(name.c_str());
    }

    if (aclVsDigit[i].GetN() > 0) {
      canvas.Clear();
      //outputFile.mkdir("clusterVsDigit");
      //outputFile.cd("clusterVsDigit");
      string name = "analogClusterVsDigit_" + component + ".png";
      aclVsDigit[i].GetXaxis()->SetTitle((component+"Digit [mm]").c_str());
      aclVsDigit[i].GetYaxis()->SetTitle((component+"AnalogCluster [mm]").c_str());
      aclVsDigit[i].GetXaxis()->SetRangeUser(axisMin, axisMax);
      aclVsDigit[i].GetYaxis()->SetRangeUser(axisMin, axisMax);
      aclVsDigit[i].Draw("ap*");
      canvas.SaveAs(name.c_str());
    }

    if (spVsHit[i].GetN() > 0) {
      canvas.Clear();
      //outputFile.mkdir("spacePointVsHit");
      //outputFile.cd("spacePointVsHit");
      string name = "spacePointVsHit_" + component + ".png";
      spVsHit[i].GetXaxis()->SetTitle((component+"Hit [mm]").c_str());
      spVsHit[i].GetYaxis()->SetTitle((component+"SpacePoint [mm]").c_str());
      spVsHit[i].GetXaxis()->SetRangeUser(axisMin, axisMax);
      spVsHit[i].GetYaxis()->SetRangeUser(axisMin, axisMax);
      spVsHit[i].Draw("ap*");
      canvas.SaveAs(name.c_str());
    }

    if (spVsCl[i].GetN() > 0) {
      canvas.Clear();
      //outputFile.mkdir("spacePointVsAnalogCluster");
      //outputFile.cd("spacePointVsAnalogCluster");
      string name = "spacePointVsAnalogCluster_" + component + ".png";
      spVsCl[i].GetXaxis()->SetTitle((component+"AnalogCluster [mm]").c_str());
      spVsCl[i].GetYaxis()->SetTitle((component+"SpacePoint [mm]").c_str());
      spVsCl[i].GetXaxis()->SetRangeUser(axisMin, axisMax);
      spVsCl[i].GetYaxis()->SetRangeUser(axisMin, axisMax);
      spVsCl[i].Draw("ap*");
      canvas.SaveAs(name.c_str());
    }
  }

  return 0;
}

