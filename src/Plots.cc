#include "FaserTracker/common_includes.hh"
#include "FaserTracker/Plots.hh"
#include "FaserTracker/DigiReader.hh"
#include "FaserTracker/TestTrackFit.hh"
#include "TFile.h"
#include "TCanvas.h"
#include "TGraph2D.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TColor.h"
#include "TLegend.h"

namespace FaserTracker {

    void Plots::plotTruthDigits(const DigiReader & digiReader, TFile & outputFile, long iEvent) {

        outputFile.cd();

        map<int, TGraph2D> trackPoints;
        map<int, TGraph>   trackPointsZX;
        map<int, TGraph>   trackPointsZY;

        shared_ptr<vector<pair<TVector3, int>>> globalPositions = digiReader.truthGlobalPositions();

        for (const auto & pos : *globalPositions) {

            int trackId = pos.second;

            if ((settings->tracks.trackIdStart > -1 && trackId <= settings->tracks.trackIdStart) ||
                (settings->tracks.trackIdEnd   > -1 && trackId >= settings->tracks.trackIdEnd))
            {
                continue;
            }

            if (trackPoints.count(trackId) < 1) {
                trackPoints[trackId]   = TGraph2D{};
                trackPointsZX[trackId] = TGraph{};
                trackPointsZY[trackId] = TGraph{};
            }

            TGraph2D & t = trackPoints[trackId];
            TGraph & tZX = trackPointsZX[trackId];
            TGraph & tZY = trackPointsZY[trackId];

            TVector3 posVec = pos.first;
            t.SetPoint(t.GetN(), posVec.X(), posVec.Y(), posVec.Z());
            tZX.SetPoint(tZX.GetN(), posVec.Z(), posVec.X());
            tZY.SetPoint(tZY.GetN(), posVec.Z(), posVec.Y());

        }

        for (auto & t : trackPoints) {
            if (t.second.GetN() < 3) continue;
            string saveName = "trackPoints_event"+to_string(iEvent)+"_track"+to_string(t.first);
            cout << "INFO  Writing TGraph2D `" << saveName << "` with " << t.second.GetN()
                 << " entries to output file track_info.root\n";
            t.second.Write(saveName.c_str());
        }

        for (auto & t : trackPointsZX) {
            if (t.second.GetN() < 3) continue;
            string saveName = "trackPointsZX_event"+to_string(iEvent)+"_track"+to_string(t.first);
            cout << "INFO  Writing TGraph `" << saveName << "` with " << t.second.GetN()
                 << " entries to output file track_info.root\n";
            //t.second.SetMarkerStyle(3);
            t.second.Write(saveName.c_str());

            if (settings->tracks.fitTracks) {
                FaserTracker::TestTrackFit::fitCircle(t.second, saveName+"_fit");
            }
        }

        for (auto & t : trackPointsZY) {
            if (t.second.GetN() < 3) continue;
            string saveName = "trackPointsZY_event"+to_string(iEvent)+"_track"+to_string(t.first);
            cout << "INFO  Writing TGraph `" << saveName << "` with " << t.second.GetN()
                 << " entries to output file track_info.root\n";
            //t.second.SetMarkerStyle(3);
            t.second.Write(saveName.c_str());
        }

    }

//    void plotTrackDigits(const std::vector<Digit> & digits, int trackId, char xAxisVar, char yAxisVar) {
//        if (xAxisVar!='x' && xAxisVar!='y' && xAxisVar!='z') {
//            cout << "ERROR  FaserTracker::Plots::plotTrackDigits\n"
//                 << "       Illegal x-axis quantity specified: " << xAxisVar << "\n";
//        }
//        if (yAxisVar!='x' && yAxisVar!='y' && yAxisVar!='z') {
//            cout << "ERROR  FaserTracker::Plots::plotTrackDigits\n"
//                 << "       Illegal y-axis quantity specified: " << yAxisVar << "\n";
//        }
//
//        auto g_digitPos = make_shared<TGraph>();
//        for (const Digit & digit : digits) {
//
//            double xVal;
//            double yVal;
//
//            switch (xAxisVar) {
//            case 'x':
//                xVal = digit.globalPosition.X();
//            case 'y':
//                xVal = digit.globalPosition.Y();
//            case 'z':
//                xVal = digit.globalPosition.Z();
//            }
//
//            switch (yAxisVar) {
//            case 'x':
//                yVal = digit.globalPosition.X();
//            case 'y':
//                yVal = digit.globalPosition.Y();
//            case 'z':
//                yVal = digit.globalPosition.Z();
//            }
//
//            g_digitPos->SetPoint(g_digitPos->GetN(), xAxisVal, yAxisVal);
//        }
//
//        return g_digitPos;
//    }
//
//    void compareDigiVsTruth(TGraph & gDigi, TGraph & gTruth) {
//        double xMin = min(gDigi.GetXaxis()->GetMinimum(), gTruth.GetXaxis()->GetMinimum());
//        double xMax = max(gDigi.GetXaxis()->GetMaximum(), gTruth.GetXaxis()->GetMaximum());
//        double yMin = min(gDigi.GetYaxis()->GetMinimum(), gTruth.GetYaxis()->GetMinimum());
//        double yMax = max(gDigi.GetYaxis()->GetMaximum(), gTruth.GetYaxis()->GetMaximum());
//        double xBuffer = 0.1 * (xMax - xMin);
//        double yBuffer = 0.1 * (yMax - yMin);
//
//        TCanvas canvas {"digiVsTruth", "digiVsTruth", 1000, 600};
//        canvas.SetGrid();
//        canvas.SetBatch(true);
//        TRandom3 r;
//        gDigi.SetMarkerColor(ROOT::kRed);
//        gDigi.Draw("ap*");
//        gDigi.GetXaxis()->SetRangeUser(xMin-xBuffer, xMax+xBuffer);
//        gDigi.GetYaxis()->SetRangeUser(yMin-yBuffer, yMax+yBuffer);
//        gTruth.SetMarkerColor(ROOT::kBlue);
//        gTruth.Draw("p*");
//
//        TLegend leg {0.2, 0.6, 0.6, 0.8};
//        leg.SetFillStyle(0);
//        leg.SetFillColor(0);
//        leg.SetBorderSize(0);
//        leg.SetTextFont(42);
//        leg.AddEntry(&gDigi, "digi", "p");
//        leg.AddEntry(&gTruth, "truth", "p");
//
//        // Add labels
//        gTruth.SetTitle("Digits vs. truth");
//        gTruth.GetXaxis()->SetTitle("z [mm]");
//        gTruth.GetYaxis()->SetTitle("x [mm]");
//
//        canvas.SaveAs("digi_vs_truth.png");
//    }

}

