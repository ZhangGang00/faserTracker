#include "FaserTracker/TrackFitter.hh"
#include "FaserTracker/common_includes.hh"

//#include <boost/program_options.hpp>
#include "Acts/Extrapolator/Navigator.hpp"
//#include "ACTFW/Common/CommonOptions.hpp"
//#include "ACTFW/Common/GeometryOptions.hpp"
//#include "ACTFW/Common/OutputOptions.hpp"
//#include "ACTFW/Framework/Sequencer.hpp"
//#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
//#include "ACTFW/Plugins/Obj/ObjPropagationStepsWriter.hpp"
//#include "ACTFW/Plugins/Root/RootPropagationStepsWriter.hpp"
//#include "ACTFW/Propagation/PropagationAlgorithm.hpp"
//#include "ACTFW/Propagation/PropagationOptions.hpp"
//#include "ACTFW/Random/RandomNumbersOptions.hpp"
//#include "ACTFW/Random/RandomNumbersSvc.hpp"
//#include "ACTFW/Utilities/Paths.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Extrapolator/Navigator.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/MagneticField/InterpolatedBFieldMap.hpp"
#include "Acts/MagneticField/SharedBFieldMap.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
//#include "Acts/Propagator/StraightLineStepper.hpp"

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
//#include "Acts/Detector/TrackingGeometry.hpp"
//#include "Acts/Extrapolator/Navigator.hpp"
//#include "Acts/MagneticField/ConstantBField.hpp"
//#include "Acts/MagneticField/InterpolatedBFieldMap.hpp"
//#include "Acts/MagneticField/SharedBFieldMap.hpp"
//#include "Acts/Propagator/EigenStepper.hpp"
//#include "Acts/Propagator/Propagator.hpp"
//#include "Acts/Propagator/StraightLineStepper.hpp"


namespace FaserTracker {

//------------------------------------------------------------------------------

TrackFitter::TrackFitter(shared_ptr<Settings> settings) {

    std::string strategy_ = settings->trackFitting.strategy;
    std::transform(strategy_.begin(), strategy_.end(), strategy_.begin(), ::tolower);

    if      (strategy_ == "global") strategy = GLOBAL;
    else if (strategy_ == "kalman") strategy = KALMAN;
    else {
        cout << "ERROR  FaserTracker::TrackFitter\n"
             << "       Invalid strategy: " << strategy_ << '\n'
             << "       Valid options are `GLOBAL` and `KALMAN`\n"
             << "       Aborting.\n";

        throw runtime_error {"FaserTracker::TrackFitter: invalid `strategy` option"};
    }

}

//------------------------------------------------------------------------------

struct TrackFitter::Impl {

    using Stepper    = Acts::EigenStepper<Acts::ConstantBField>;
    using Propagator = Acts::Propagator<Stepper, Acts::Navigator>;

    shared_ptr<const Acts::TrackingGeometry> geo;
    shared_ptr<Propagator> propagator;

    Impl()
        //: geo {make_unique<Acts::Geometry>()}
    {
        // Set up ACTS machinery
        //TODO: Acts::??? trackingGeo = ???;
        //Acts::Navigator navigator {trackingGeo};

        //TODO: read this from geo config instead of hardcoding
        Stepper    stepper { {0., 0.5, 0.} };
        //Propagator propagator {std::move(stepper), std::move(navigator)};
    }

    FittedTrack fitTrackCandidate_global(const TrackCandidate & candidate);

    FittedTrack fitTrackCandidate_kalman(const TrackCandidate & candidate);

};

//------------------------------------------------------------------------------

FittedTrack TrackFitter::fitTrackCandidate(TrackCandidate & candidate) {

    candidate.sortSpacePointsByZ();

    switch (strategy) {
    case GLOBAL:
        return impl->fitTrackCandidate_global(candidate);
        break;
    case KALMAN:
        return impl->fitTrackCandidate_kalman(candidate);
        break;
    }

}

//------------------------------------------------------------------------------
// Implementation details

FittedTrack TrackFitter::Impl::fitTrackCandidate_global(const TrackCandidate & candidate) {
    FittedTrack track;
    return track;
}

//------------------------------------------------------------------------------

FittedTrack TrackFitter::Impl::fitTrackCandidate_kalman(const TrackCandidate & candidate) {


    FittedTrack track;
    return track;

}

//------------------------------------------------------------------------------

}



////////////////////////////////////////////////////////////////////////////////

//namespace FaserTracker {
//
//    FittedTrack TrackFitter::fitTrackCandidate(const TrackCandidate & trackCandidate) {
//
//        TGraph g_clusters;
//        for (const DigiCluster & cluster : *trackCandidate.digiClusters) {
//            shared_ptr<TVector3> pos = cluster.globalPosition();
//            g_clusters.SetPoint(g_clusters.GetN(), pos->Z(), pos->X());
//        }
//
//        TCanvas canvas {"fittedTrack", "fittedTrack", 1000, 600};
//        canvas.SetGrid();
//        canvas.SetBatch(true);
//        TRandom3 r;
//        g_clusters.Draw("ap*");
//        auto chi2Function = [&](const Double_t *par) {
//            //minimisation function computing the sum of squares of residuals
//            // looping at the graph points
//            double f = 0;
//            double *x = g_clusters.GetX();
//            double *y = g_clusters.GetY();
//            for (int i=0; i<g_clusters.GetN(); ++i) {
//                double u = x[i] - par[0];
//                double v = y[i] - par[1];
//                double dr = par[2] - std::sqrt(u*u+v*v);
//                f += dr*dr;
//            }
//            return f;
//        };
//        // wrap chi2 funciton in a function object for the fit
//        // 3 is the number of fit parameters (size of array par)
//        ROOT::Math::Functor fcn(chi2Function, 3);
//        ROOT::Fit::Fitter  fitter;
//        double pStart[3] = {0,0,1};
//        fitter.SetFCN(fcn, pStart);
//        fitter.Config().ParSettings(0).SetName("z0");
//        fitter.Config().ParSettings(1).SetName("x0");
//        fitter.Config().ParSettings(2).SetName("R");
//        // do the fit 
//        bool ok = fitter.FitFCN();
//        if (!ok) {
//            Error("line3Dfit","Line3D Fit failed");
//        }   
//        const ROOT::Fit::FitResult & result = fitter.Result();
//        result.Print(cout);
//        //Draw the circle on top of the points
//        //TArc *arc = new TArc(result.Parameter(0),result.Parameter(1),result.Parameter(2));
//        //arc->SetLineColor(kRed);
//        //arc->SetLineWidth(4);
//        //arc->Draw();
//        double z0 = result.Parameter(0);
//        double x0 = result.Parameter(1);
//        double R  = result.Parameter(2);
//        double zMin = TMath::MinElement(g_clusters.GetN(), g_clusters.GetX());
//        double zMax = TMath::MaxElement(g_clusters.GetN(), g_clusters.GetX());
//
//        TF1 f_upperSemiCircle {"f_upperSemiCircle", "[1] + sqrt([2]*[2] - (x-[0])*(x-[0]))", zMin, zMax};
//        f_upperSemiCircle.SetParameter(0, z0);
//        f_upperSemiCircle.SetParameter(1, x0);
//        f_upperSemiCircle.SetParameter(2, R);
//        f_upperSemiCircle.Draw("same");
//
//        TF1 f_lowerSemiCircle {"f_lowerSemiCircle", "[1] - sqrt([2]*[2] - (x-[0])*(x-[0]))", zMin, zMax};
//        f_lowerSemiCircle.SetParameter(0, z0);
//        f_lowerSemiCircle.SetParameter(1, x0);
//        f_lowerSemiCircle.SetParameter(2, R);
//        f_lowerSemiCircle.Draw("same");
//
//        // Add labels
//        g_clusters.SetTitle(trackCandidate.label.c_str());
//        g_clusters.GetXaxis()->SetTitle("z [mm]");
//        g_clusters.GetYaxis()->SetTitle("x [mm]");
//
//        string saveName = "fittedTrack_" + trackCandidate.label + ".png";
//        canvas.SaveAs(saveName.c_str());
//    }
//
//}
//
