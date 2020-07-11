// adapted from Geant4 example

#include "FaserTracker/TrackCandidate.hh"
#include "FaserTracker/TrackFinder.hh"
#include "FaserTracker/TrackFitter.hh"
#include "FaserTracker/common_includes.hh"

#include "FaserTrackerGeometry.hh"
#include "FaserTrackerEvent.hh"
#include "FaserTrackerSpacePoint.hh"
#include "FaserTrackerTruthParticle.hh"
#include "FaserTrackerTruthHit.hh"

#include <cstdlib>
#include <vector>

#include "TROOT.h"
#include "TSystem.h"
#include "TChain.h"
#include "TFile.h"
#include "TGraph2D.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TMinuit.h"

static double target_z[8]={0, 50, 909.04, 959.04, 1009.04, 1059.04, 1942.02, 1992.02};

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

int helix(vector <double>& outvect, double px, double py, double pz, double x0, double y0, double z0, double B, int charge, double target_z, double s)
{
  if (target_z==-9999 && s==-9999) {cout<<"Error!!!   Please clarify what to calculate"<<std::endl; return -1; }
  
  double p_perp = sqrt(px*px + pz*pz);  // The magnet field point to positive y axis
  double sin_lambda = py/sqrt(p_perp*p_perp + py*py); 
  double cos_lambda = p_perp/sqrt(p_perp*p_perp + py*py); 
  double R = 1000*p_perp/(0.3*B);  // R(mm), p(GeV), B(T)
  double vRx = -charge*pz/p_perp;  // vR is a norm vecter pointing from intial position to the center of the circle
  double vRz = charge*px/p_perp;
  double Cx = x0 + R*vRx;  // (Cx, Cz) are the coordinates of center of the circle  
  double Cz = z0 + R*vRz;
  double phi=0;  // -vR determines phi
  if (-vRx > 0) phi = std::asin(-vRz);
    else if (-vRz >0) phi = TMath::Pi() - std::asin(-vRz);
    else phi =  -TMath::Pi() - std::asin(-vRz);

  if (s != -9999) {
    outvect[0] = Cx + R*cos(phi+charge*s*cos_lambda/R);  //x 
    outvect[1] = y0 + s*sin_lambda;  //y
    outvect[2] = Cz + R*sin(phi+charge*s*cos_lambda/R);  //z
    //cout<<"SSS  CxCz"<<setprecision(9)<<Cx<<"  "<<Cz<<"  R="<<R<<"  phi="<<phi<<"  "<<charge*s*cos_lambda/R<<"  "<<sin(phi+charge*s*cos_lambda/R)<<"  "<<Cz + R*sin(phi+charge*s*cos_lambda/R)<<"  "<<outvect[2]<<std::endl;
    outvect[3] = -charge*p_perp*sin(phi+charge*s*cos_lambda/R); //px
    outvect[4] = py;  //py
    outvect[5] = charge*p_perp*cos(phi+charge*s*cos_lambda/R); //pz
  }
	
  if (target_z != -9999) {
    if (target_z-Cz > R) 
      {
	 cout<<"Warning!!!  target_z is outside the circle. target_z="<<target_z<<",  Cz="<<Cz<<",  R="<<R<<std::endl; 
	 return -1;
      }
    double target_s=0;
    if (charge==1)  target_s = (std::asin((target_z - Cz)/R) - phi)*R/(charge*cos_lambda);
    else if (-vRz >0) target_s = ((TMath::Pi()-std::asin((target_z - Cz)/R)) - phi)*R/(charge*cos_lambda);
    else target_s = ((-TMath::Pi()-std::asin((target_z - Cz)/R)) - phi)*R/(charge*cos_lambda);
    if (target_s<0) {cout<<"Error!!!  Negtive target_s !!!"<<std::endl; return -1; }
    outvect[0] = Cx + R*cos(phi+charge*target_s*cos_lambda/R); 
    outvect[1] = y0 + target_s*sin_lambda;
    outvect[2] = target_z;
    outvect[3] = -charge*p_perp*sin(phi+charge*target_s*cos_lambda/R); 
    outvect[4] = py;
    outvect[5] = charge*p_perp*cos(phi+charge*target_s*cos_lambda/R); 
  }

  return 0; 
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

    TFile inputFile {input.c_str()};

    TTree * eventTree = (TTree*) inputFile.Get("events");
    if (eventTree == nullptr) {
        cout << "ERROR  Unable to load event tree: `events`\n"
             << "       Exiting.\n"
             << "\n";
        return 1;
    }
    cout << "INFO  Loaded event tree `events` from "
         << input << " with " << eventTree->GetEntries() << " entries.\n";
#if 0
    TTree * geoTree = (TTree*) inputFile.Get("geo");
    if (geoTree == nullptr) {
        cout << "ERROR  Unable to load event tree: `geo`\n"
             << "       Exiting.\n"
             << "\n";
        return 1;
    }
    long nGeoEntries = geoTree->GetEntries();
    cout << "INFO  Loaded input geometry tree `geo` from "
         << input << " with " << nGeoEntries << " entries.\n";
    if (nGeoEntries != 1) {
        cout << "ERROR  The geometry tree `geo` has " << nGeoEntries << " entries.\n"
             << "       It should have exactly one entry. Exiting.\n"
             << "\n";
        return 1;
    }
    

    FaserTrackerGeometry * geo = nullptr;
    geoTree->SetBranchAddress("geo", &geo);
    geoTree->GetEntry(0);
#endif

    TGraphErrors *g0_x = new TGraphErrors();
    TGraphErrors *g1_x = new TGraphErrors();
    TGraphErrors *g2_x = new TGraphErrors();
    TGraphErrors *g3_x = new TGraphErrors();
    TGraphErrors *g4_x = new TGraphErrors();
    TGraphErrors *g5_x = new TGraphErrors();
    TGraphErrors *g6_x = new TGraphErrors();
    TGraphErrors *g7_x = new TGraphErrors();
    TGraphErrors *g0_y = new TGraphErrors();
    TGraphErrors *g1_y = new TGraphErrors();
    TGraphErrors *g2_y = new TGraphErrors();
    TGraphErrors *g3_y = new TGraphErrors();
    TGraphErrors *g4_y = new TGraphErrors();
    TGraphErrors *g5_y = new TGraphErrors();
    TGraphErrors *g6_y = new TGraphErrors();
    TGraphErrors *g7_y = new TGraphErrors();
    int count[8]={0};
    double truth_px=0,truth_py=0,truth_pz=0;
    int charge=-1;
    double B=0.5;
    std::vector <double> outvect(6,0);
    double truth_x[8]={0};
    double truth_y[8]={0};

    FaserTrackerEvent * event = nullptr;
    eventTree->SetBranchAddress("events", &event);
    //for (int iEntry=0; iEntry<eventTree->GetEntries(); ++iEntry) 
    for (int iEntry=0; iEntry<200; ++iEntry) 
     {
        eventTree->GetEntry(iEntry);
        cout << "INFO  Processing event " << event->eventNumber << "\n";

	int E=0,index_tp=0,index_sp=0,index_th=0;
        for (const FaserTrackerTruthParticle * tp : event->truthParticles) 
	  {
	    //if (tp->pdgCode==13 && tp->parentID==1)
	    //if (tp->pdgCode==13 )
	     {
               const TVector3 & ver = tp->vertex;
	       const TLorentzVector & fmom = tp->fourMomentum;

	       index_tp++;
	       cout<<index_tp<<"  parentID="<<tp->parentID<<"  "<<"pdgCode="<<tp->pdgCode<<"  trackID="<<tp->trackID<<"  coodinates="<<ver.X()<<"  "<<ver.Y()<<"  "<<ver.Z()<<"  momemtum="<<fmom.Px()/1000.<<"  "<<fmom.Py()/1000.<<"  "<<fmom.Pz()/1000.<<"  "<<fmom.E()/1000.0<<"\n";
	       truth_px=fmom.Px()/1000.; truth_py=fmom.Py()/1000.; truth_pz=fmom.Pz()/1000.; 	
	//cout<<"Find Muon!!! coordinates="<<ver.X()<<"  "<<ver.Y()<<"  "<<ver.Z()<<"  momemtum="<<truth_px<<"  "<<truth_py<<"  "<<truth_pz<<"\n";
#if 0
	       if ( helix(outvect,truth_px,truth_py,truth_pz,ver.X(),ver.Y(),ver.Z(),B,charge,target_z[0],-9999) == -1 ) E=1;
	       truth_x[0]=outvect[0]; truth_y[0]=outvect[1];
	       if ( helix(outvect,truth_px,truth_py,truth_pz,ver.X(),ver.Y(),ver.Z(),B,charge,target_z[1],-9999) == -1 ) E=1;
	       truth_x[1]=outvect[0]; truth_y[1]=outvect[1];
	       if ( helix(outvect,truth_px,truth_py,truth_pz,ver.X(),ver.Y(),ver.Z(),B,charge,target_z[2],-9999) == -1 ) E=1;
	       truth_x[2]=outvect[0]; truth_y[2]=outvect[1];
	       if ( helix(outvect,truth_px,truth_py,truth_pz,ver.X(),ver.Y(),ver.Z(),B,charge,target_z[3],-9999) == -1 ) E=1;
	       truth_x[3]=outvect[0]; truth_y[3]=outvect[1];
	       if ( helix(outvect,truth_px,truth_py,truth_pz,ver.X(),ver.Y(),ver.Z(),B,charge,target_z[4],-9999) == -1 ) E=1;
	       truth_x[4]=outvect[0]; truth_y[4]=outvect[1];
	       if ( helix(outvect,truth_px,truth_py,truth_pz,ver.X(),ver.Y(),ver.Z(),B,charge,target_z[5],-9999) == -1 ) E=1;
	       truth_x[5]=outvect[0]; truth_y[5]=outvect[1];
	       if ( helix(outvect,truth_px,truth_py,truth_pz,ver.X(),ver.Y(),ver.Z(),B,charge,target_z[6],-9999) == -1 ) E=1;
	       truth_x[6]=outvect[0]; truth_y[6]=outvect[1];
	       if ( helix(outvect,truth_px,truth_py,truth_pz,ver.X(),ver.Y(),ver.Z(),B,charge,target_z[7],-9999) == -1 ) E=1;
	       truth_x[7]=outvect[0]; truth_y[7]=outvect[1];
#endif
	     }
	  }

	if (E==1) continue;

        for (const FaserTrackerSpacePoint * sp : event->spacePoints) {
            const TVector3 & pos = sp->globalPos;
            const TVector3 & posUncert = sp->globalPosUncertainty;
	    if (std::fabs(pos.Z()-target_z[0])<1) 
	     { 
	        g0_x->SetPoint(count[0],truth_x[0],pos.X());
		g0_x->SetPointError(count[0],0,posUncert.X());
		//g0_x->SetPointError(count[0],0.016,0);
	        g0_y->SetPoint(count[0],truth_y[0],pos.Y());
		g0_y->SetPointError(count[0],0,posUncert.Y());
		//g0_y->SetPointError(count[0],0.816,0);
		count[0]++;
	     }
	    if (std::fabs(pos.Z()-target_z[1])<1) 
	     { 
	        g1_x->SetPoint(count[1],truth_x[1],pos.X());
		g1_x->SetPointError(count[1],0,posUncert.X());
	        g1_y->SetPoint(count[1],truth_y[1],pos.Y());
		g1_y->SetPointError(count[1],0,posUncert.Y());
		count[1]++;
	     }
	    if (std::fabs(pos.Z()-target_z[2])<1) 
	     { 
	        g2_x->SetPoint(count[2],truth_x[2],pos.X());
		g2_x->SetPointError(count[2],0,posUncert.X());
	        g2_y->SetPoint(count[2],truth_y[2],pos.Y());
		g2_y->SetPointError(count[2],0,posUncert.Y());
		count[2]++;
	     }
	    if (std::fabs(pos.Z()-target_z[3])<1) 
	     { 
	        g3_x->SetPoint(count[3],truth_x[3],pos.X());
		g3_x->SetPointError(count[3],0,posUncert.X());
	        g3_y->SetPoint(count[3],truth_y[3],pos.Y());
		g3_y->SetPointError(count[3],0,posUncert.Y());
		count[3]++;
	     }
	    if (std::fabs(pos.Z()-target_z[4])<1) 
	     { 
	        g4_x->SetPoint(count[4],truth_x[4],pos.X());
		g4_x->SetPointError(count[4],0,posUncert.X());
	        g4_y->SetPoint(count[4],truth_y[4],pos.Y());
		g4_y->SetPointError(count[4],0,posUncert.Y());
		count[4]++;
	     }
	    if (std::fabs(pos.Z()-target_z[5])<1) 
	     { 
	        g5_x->SetPoint(count[5],truth_x[5],pos.X());
		g5_x->SetPointError(count[5],0,posUncert.X());
	        g5_y->SetPoint(count[5],truth_y[5],pos.Y());
		g5_y->SetPointError(count[5],0,posUncert.Y());
		count[5]++;
	     }
	    if (std::fabs(pos.Z()-target_z[6])<1) 
	     { 
	        g6_x->SetPoint(count[6],truth_x[6],pos.X());
		g6_x->SetPointError(count[6],0,posUncert.X());
	        g6_y->SetPoint(count[6],truth_y[6],pos.Y());
		g6_y->SetPointError(count[6],0,posUncert.Y());
		count[6]++;
	     }
	    if (std::fabs(pos.Z()-target_z[7])<1) 
	     { 
	        g7_x->SetPoint(count[7],truth_x[7],pos.X());
		g7_x->SetPointError(count[7],0,posUncert.X());
	        g7_y->SetPoint(count[7],truth_y[7],pos.Y());
		g7_y->SetPointError(count[7],0,posUncert.Y());
		count[7]++;
	     }
	    //cout<<"GetN="<<g0_x->GetN()<<"  count="<<count[0]<<"\n";
	    index_sp++;
            cout <<"space point="<<index_sp<<"  plane="<<sp->plane<<"  module="<<sp->module<<"  sensor="<<sp->sensor<<"  row="<<sp->row<<"  charge="<<sp->charge<<"  coordinate="<< pos.X() << " " << pos.Y() << " " << pos.Z() << "\n";
        }

#if 1
        for (const FaserTrackerTruthHit * th : event->truthHits) {
            const TVector3 & pos = th->globalPos;
	    index_th++;
            cout <<"truth hit="<<index_th<<"  trackID="<<th->trackID<<"  plane="<<th->plane<<"  module="<<th->module<<"  sensor="<<th->sensor<<"  row="<<th->row<<"  strip="<<th->strip<<"  coordinate="<< pos.X() << " " << pos.Y() << " " << pos.Z() << "\n";
	}
#endif
    }

    TCanvas *c1 = new TCanvas("c1","", 800,600);
    c1->cd();
    g0_x->SetMarkerStyle(20);
    g0_x->SetMarkerSize(0.5);
    g0_x->Draw("AP");
    g0_x->GetXaxis()->SetRangeUser(-100,100);
    g0_x->GetYaxis()->SetRangeUser(-100,100);
    g0_x->GetYaxis()->SetTitle("space point X (mm)");
    g0_x->GetXaxis()->SetTitle("truth X (mm)");
    c1->Update();
    c1->SaveAs("/afs/cern.ch/user/g/gang/FASER/faser_tracker/plots/plane0_X.png");
    g0_y->SetMarkerStyle(20);
    g0_y->SetMarkerSize(0.5);
    g0_y->Draw("AP");
    g0_y->GetXaxis()->SetRangeUser(-100,100);
    g0_y->GetYaxis()->SetRangeUser(-100,100);
    g0_y->GetYaxis()->SetTitle("space point Y (mm)");
    g0_y->GetXaxis()->SetTitle("truth Y (mm)");
    c1->Update();
    c1->SaveAs("/afs/cern.ch/user/g/gang/FASER/faser_tracker/plots/plane0_Y.png");

    g1_x->SetMarkerStyle(20);
    g1_x->SetMarkerSize(0.5);
    g1_x->Draw("AP");
    g1_x->GetXaxis()->SetRangeUser(-100,100);
    g1_x->GetYaxis()->SetRangeUser(-100,100);
    g1_x->GetYaxis()->SetTitle("space point X (mm)");
    g1_x->GetXaxis()->SetTitle("truth X (mm)");
    c1->Update();
    c1->SaveAs("/afs/cern.ch/user/g/gang/FASER/faser_tracker/plots/plane1_X.png");
    g1_y->SetMarkerStyle(20);
    g1_y->SetMarkerSize(0.5);
    g1_y->Draw("AP");
    g1_y->GetXaxis()->SetRangeUser(-100,100);
    g1_y->GetYaxis()->SetRangeUser(-100,100);
    g1_y->GetYaxis()->SetTitle("space point Y (mm)");
    g1_y->GetXaxis()->SetTitle("truth Y (mm)");
    c1->Update();
    c1->SaveAs("/afs/cern.ch/user/g/gang/FASER/faser_tracker/plots/plane1_Y.png");

    g2_x->SetMarkerStyle(20);
    g2_x->SetMarkerSize(0.5);
    g2_x->Draw("AP");
    g2_x->GetXaxis()->SetRangeUser(-100,100);
    g2_x->GetYaxis()->SetRangeUser(-100,100);
    g2_x->GetYaxis()->SetTitle("space point X (mm)");
    g2_x->GetXaxis()->SetTitle("truth X (mm)");
    c1->Update();
    c1->SaveAs("/afs/cern.ch/user/g/gang/FASER/faser_tracker/plots/plane2_X.png");
    g2_y->SetMarkerStyle(20);
    g2_y->SetMarkerSize(0.5);
    g2_y->Draw("AP");
    g2_y->GetXaxis()->SetRangeUser(-100,100);
    g2_y->GetYaxis()->SetRangeUser(-100,100);
    g2_y->GetYaxis()->SetTitle("space point Y (mm)");
    g2_y->GetXaxis()->SetTitle("truth Y (mm)");
    c1->Update();
    c1->SaveAs("/afs/cern.ch/user/g/gang/FASER/faser_tracker/plots/plane2_Y.png");

    g3_x->SetMarkerStyle(20);
    g3_x->SetMarkerSize(0.5);
    g3_x->Draw("AP");
    g3_x->GetXaxis()->SetRangeUser(-100,100);
    g3_x->GetYaxis()->SetRangeUser(-100,100);
    g3_x->GetYaxis()->SetTitle("space point X (mm)");
    g3_x->GetXaxis()->SetTitle("truth X (mm)");
    c1->Update();
    c1->SaveAs("/afs/cern.ch/user/g/gang/FASER/faser_tracker/plots/plane3_X.png");
    g3_y->SetMarkerStyle(20);
    g3_y->SetMarkerSize(0.5);
    g3_y->Draw("AP");
    g3_y->GetXaxis()->SetRangeUser(-100,100);
    g3_y->GetYaxis()->SetRangeUser(-100,100);
    g3_y->GetYaxis()->SetTitle("space point Y (mm)");
    g3_y->GetXaxis()->SetTitle("truth Y (mm)");
    c1->Update();
    c1->SaveAs("/afs/cern.ch/user/g/gang/FASER/faser_tracker/plots/plane3_Y.png");

    g4_x->SetMarkerStyle(20);
    g4_x->SetMarkerSize(0.5);
    g4_x->Draw("AP");
    g4_x->GetXaxis()->SetRangeUser(-100,100);
    g4_x->GetYaxis()->SetRangeUser(-100,100);
    g4_x->GetYaxis()->SetTitle("space point X (mm)");
    g4_x->GetXaxis()->SetTitle("truth X (mm)");
    c1->Update();
    c1->SaveAs("/afs/cern.ch/user/g/gang/FASER/faser_tracker/plots/plane4_X.png");
    g4_y->SetMarkerStyle(20);
    g4_y->SetMarkerSize(0.5);
    g4_y->Draw("AP");
    g4_y->GetXaxis()->SetRangeUser(-100,100);
    g4_y->GetYaxis()->SetRangeUser(-100,100);
    g4_y->GetYaxis()->SetTitle("space point Y (mm)");
    g4_y->GetXaxis()->SetTitle("truth Y (mm)");
    c1->Update();
    c1->SaveAs("/afs/cern.ch/user/g/gang/FASER/faser_tracker/plots/plane4_Y.png");

    g5_x->SetMarkerStyle(20);
    g5_x->SetMarkerSize(0.5);
    g5_x->Draw("AP");
    g5_x->GetXaxis()->SetRangeUser(-100,100);
    g5_x->GetYaxis()->SetRangeUser(-100,100);
    g5_x->GetYaxis()->SetTitle("space point X (mm)");
    g5_x->GetXaxis()->SetTitle("truth X (mm)");
    c1->Update();
    c1->SaveAs("/afs/cern.ch/user/g/gang/FASER/faser_tracker/plots/plane5_X.png");
    g5_y->SetMarkerStyle(20);
    g5_y->SetMarkerSize(0.5);
    g5_y->Draw("AP");
    g5_y->GetXaxis()->SetRangeUser(-100,100);
    g5_y->GetYaxis()->SetRangeUser(-100,100);
    g5_y->GetYaxis()->SetTitle("space point Y (mm)");
    g5_y->GetXaxis()->SetTitle("truth Y (mm)");
    c1->Update();
    c1->SaveAs("/afs/cern.ch/user/g/gang/FASER/faser_tracker/plots/plane5_Y.png");

    g6_x->SetMarkerStyle(20);
    g6_x->SetMarkerSize(0.5);
    g6_x->Draw("AP");
    g6_x->GetXaxis()->SetRangeUser(-100,100);
    g6_x->GetYaxis()->SetRangeUser(-100,100);
    g6_x->GetYaxis()->SetTitle("space point X (mm)");
    g6_x->GetXaxis()->SetTitle("truth X (mm)");
    c1->Update();
    c1->SaveAs("/afs/cern.ch/user/g/gang/FASER/faser_tracker/plots/plane6_X.png");
    g6_y->SetMarkerStyle(20);
    g6_y->SetMarkerSize(0.5);
    g6_y->Draw("AP");
    g6_y->GetXaxis()->SetRangeUser(-100,100);
    g6_y->GetYaxis()->SetRangeUser(-100,100);
    g6_y->GetYaxis()->SetTitle("space point Y (mm)");
    g6_y->GetXaxis()->SetTitle("truth Y (mm)");
    c1->Update();
    c1->SaveAs("/afs/cern.ch/user/g/gang/FASER/faser_tracker/plots/plane6_Y.png");

    g7_x->SetMarkerStyle(20);
    g7_x->SetMarkerSize(0.5);
    g7_x->Draw("AP");
    g7_x->GetXaxis()->SetRangeUser(-100,100);
    g7_x->GetYaxis()->SetRangeUser(-100,100);
    g7_x->GetYaxis()->SetTitle("space point X (mm)");
    g7_x->GetXaxis()->SetTitle("truth X (mm)");
    c1->Update();
    c1->SaveAs("/afs/cern.ch/user/g/gang/FASER/faser_tracker/plots/plane7_X.png");
    g7_y->SetMarkerStyle(20);
    g7_y->SetMarkerSize(0.5);
    g7_y->Draw("AP");
    g7_y->GetXaxis()->SetRangeUser(-100,100);
    g7_y->GetYaxis()->SetRangeUser(-100,100);
    g7_y->GetYaxis()->SetTitle("space point Y (mm)");
    g7_y->GetXaxis()->SetTitle("truth Y (mm)");
    c1->Update();
    c1->SaveAs("/afs/cern.ch/user/g/gang/FASER/faser_tracker/plots/plane7_Y.png");

    delete c1;
}

