#include "FaserTracker/IO.hh"
#include "FaserTracker/Plots.hh"
#include "FaserTracker/common_includes.hh"
#include "FaserTrackerEvent.hh"
#include <cstdlib>
#include "TROOT.h"
#include "TSystem.h"
#include "TChain.h"
#include "TFile.h"
#include "TGraph2D.h"
#include "TGraph.h"

using namespace FaserTracker;


//------------------------------------------------------------------------------

void printUsage(string executable) {
    cout << "Usage: " << executable << " [options]\n"
         << "Options:\n"
         << "  -h/--help                      print help\n"
         << "  -i/--input FILE_NAME           path to input ROOT file\n"
         << "  -n/--nEntries N_ENTRIES        number of entries from input to process\n"
         << '\n';
}

//------------------------------------------------------------------------------

int main(int argc, char ** argv) {
    string input = "";
    long   nEntries = -1;

    int iOpt = 1;
    while (iOpt < argc) {
        string sw = argv[iOpt];
        if      (sw=="-h" || sw=="--help"      ) { printUsage(argv[0]); return 0; }
        else if (sw=="-i" || sw=="--input"     ) { input = argv[++iOpt]; }
        else if (sw=="-n" || sw=="--nEntries"  ) { nEntries = stoi(argv[++iOpt]); }
        else {
            cout << "Unknown switch: " << sw << '\n';
            printUsage(argv[0]);
            return 1;
        }
        ++iOpt;
    }
    cout << "Flags:\n"
         << "------\n"
         << "  nEntries = " << nEntries << '\n'
         << "  input    = " << input    << '\n'
         << '\n';


    // Load settings

    IO io {input};
    FaserTrackerEvent * event = nullptr;
    io.eventTree()->SetBranchAddress("events", &event);
    for (int iEntry=0; iEntry<io.eventTree()->GetEntries(); ++iEntry) {
        io.eventTree()->GetEntry(iEntry);
        cout << "INFO  Processing event " << event->eventNumber << '\n';
        for (const FaserTrackerSpacePoint * sp : event->spacePoints) {
            const TVector3 & pos = sp->globalPos;
            cout << "      Read space point with global position (" << pos.X() << "," << pos.Y() << "," << pos.Z() << ")\n";
        }
    }

}

