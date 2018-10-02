#include "FaserTracker/common_includes.hh"
#include "FaserTracker/TrackFitter.hh"
#include "TVector3.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "TRandom3.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TF1.h"
#include "TMath.h"
#include "TArc.h"
#include "Math/Functor.h"
#include "Fit/Fitter.h"


namespace FaserTracker {

    void TrackFitter::fitAndSaveCircularXZ(const TrackCandidate & trackCandidate) {

        TGraph g_spacePoints;
        for (const SpacePoint & sp : trackCandidate.spacePoints) {
            const TVector3 &  pos = sp.globalPos;
            g_spacePoints.SetPoint(g_spacePoints.GetN(), pos.Z(), pos.X());
        }

        TCanvas canvas {"fittedTrack", "fittedTrack", 1000, 600};
        canvas.SetGrid();
        canvas.SetBatch(true);
        TRandom3 r;
        g_spacePoints.Draw("ap*");
        auto chi2Function = [&](const Double_t *par) {
            //minimisation function computing the sum of squares of residuals
            // looping at the graph points
            double f = 0;
            double *x = g_spacePoints.GetX();
            double *y = g_spacePoints.GetY();
            for (int i=0; i<g_spacePoints.GetN(); ++i) {
                double u = x[i] - par[0];
                double v = y[i] - par[1];
                double dr = par[2] - std::sqrt(u*u+v*v);
                f += dr*dr;
            }
            return f;
        };
        // wrap chi2 funciton in a function object for the fit
        // 3 is the number of fit parameters (size of array par)
        ROOT::Math::Functor fcn(chi2Function, 3);
        ROOT::Fit::Fitter  fitter;
        double pStart[3] = {0,0,1};
        fitter.SetFCN(fcn, pStart);
        fitter.Config().ParSettings(0).SetName("z0");
        fitter.Config().ParSettings(1).SetName("x0");
        fitter.Config().ParSettings(2).SetName("R");
        // do the fit 
        bool ok = fitter.FitFCN();
        if (!ok) {
            Error("line3Dfit","Line3D Fit failed");
        }   
        const ROOT::Fit::FitResult & result = fitter.Result();
        result.Print(cout);
        //Draw the circle on top of the points
        //TArc *arc = new TArc(result.Parameter(0),result.Parameter(1),result.Parameter(2));
        //arc->SetLineColor(kRed);
        //arc->SetLineWidth(4);
        //arc->Draw();
        double z0 = result.Parameter(0);
        double x0 = result.Parameter(1);
        double R  = result.Parameter(2);
        double zMin = TMath::MinElement(g_spacePoints.GetN(), g_spacePoints.GetX());
        double zMax = TMath::MaxElement(g_spacePoints.GetN(), g_spacePoints.GetX());

        TF1 f_upperSemiCircle {"f_upperSemiCircle", "[1] + sqrt([2]*[2] - (x-[0])*(x-[0]))", zMin, zMax};
        f_upperSemiCircle.SetParameter(0, z0);
        f_upperSemiCircle.SetParameter(1, x0);
        f_upperSemiCircle.SetParameter(2, R);
        f_upperSemiCircle.Draw("same");

        TF1 f_lowerSemiCircle {"f_lowerSemiCircle", "[1] - sqrt([2]*[2] - (x-[0])*(x-[0]))", zMin, zMax};
        f_lowerSemiCircle.SetParameter(0, z0);
        f_lowerSemiCircle.SetParameter(1, x0);
        f_lowerSemiCircle.SetParameter(2, R);
        f_lowerSemiCircle.Draw("same");

        // Add labels
        g_spacePoints.SetTitle(trackCandidate.label.c_str());
        g_spacePoints.GetXaxis()->SetTitle("z [mm]");
        g_spacePoints.GetYaxis()->SetTitle("x [mm]");

        string saveName = "fittedTrack_" + trackCandidate.label + ".png";
        canvas.SaveAs(saveName.c_str());
    }

}

