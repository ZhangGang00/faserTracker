#include "FaserTracker/TrackCandidate.hh"
#include "FaserTracker/TrackFinder.hh"
#include "FaserTracker/TrackFitter.hh"
#include "FaserTracker/common_includes.hh"

#include "FaserTrackerGeometry.hh"
#include "FaserTrackerEvent.hh"
#include "FaserTrackerSpacePoint.hh"
#include "FaserTrackerTruthParticle.hh"
#include "FaserTrackerDigit.hh"
#include "FaserTrackerCluster.hh"
#include "FaserTrackerTruthHit.hh"

#include <cstdlib>
#include <vector>

#include "TROOT.h"
#include "TSystem.h"
#include "TChain.h"
#include "TTree.h"
#include "TFile.h"
#include "TMinuit.h"
#include "TMath.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TGraph2D.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TH1.h"

static std::vector<TVector3> selected_track_point;
static const double B=0.6;
static const int N_plane=9; 
//static double target_z[8]={0, 50, 909.04, 959.04, 1009.04, 1059.04, 1942.02, 1992.02};
static double target_z[9]={0, 50, 100, 938.03, 988.03, 1038.03, 1892.02, 1942.02, 1992.02};   //ITK
//static double target_z[9]={0, 50, 100, 947, 997, 1047, 1898, 1948, 1998};  //SCT
static int charge=-1;
static int target_pdgID=11;  // 11(e-) or 13(mu-)

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

int helix(vector<double>& outvect, double px, double py, double pz, double x0, double y0, double z0, double B, int charge, double target_z, double s)
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
    if (target_s<-0.001) {cout<<"Error!!!  Negtive target_s !!!"<<std::endl; return -1; }
    outvect[0] = Cx + R*cos(phi+charge*target_s*cos_lambda/R); 
    outvect[1] = y0 + target_s*sin_lambda;
    outvect[2] = target_z;
    outvect[3] = -charge*p_perp*sin(phi+charge*target_s*cos_lambda/R); 
    outvect[4] = py;
    outvect[5] = charge*p_perp*cos(phi+charge*target_s*cos_lambda/R); 
  }

  return 1; 
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

   FaserTrackerEvent * event = nullptr;
   eventTree->SetBranchAddress("events", &event);

    TFile *outf = new TFile("/afs/cern.ch/user/g/gang/FASER/faser_tracker/plot_truth_output.root","recreate");
    TTree *mytree = new TTree("tracks","");
    double truth_px,truth_py,truth_pz,truth_E,truth_R,truth_phi0,truth_lambda;
    double dark_photon_px,dark_photon_py,dark_photon_pz,dark_photon_E,lep0_px,lep0_py,lep0_pz,lep0_E,lep1_px,lep1_py,lep1_pz,lep1_E,decay_x,decay_y,decay_z,lep_plus_E, lep_minus_E;
    std::vector<double> truth_hits_x;
    std::vector<double> truth_hits_y;
    std::vector<double> truth_hits_z;
    std::vector<double> truth_x;
    std::vector<double> truth_y;
    std::vector<double> truth_z;
    double reco_px,reco_py,reco_pz,reco_E,reco_R,reco_phi0,reco_lambda;
    std::vector<double> reco_x;
    std::vector<double> reco_y;
    std::vector<double> reco_z;
    double chi_square;
    int N_track_points,pdgID,N_shared_sp,status;
    std::vector<double> space_points_x;
    std::vector<double> space_points_y;
    std::vector<double> space_points_z;
    std::vector<double> track_points_x;
    std::vector<double> track_points_y;
    std::vector<double> track_points_z;
    std::vector<int> track_points_rowID;
    std::vector<int> Is_shared_sp;
    std::vector<double> d_truth_hits;
    std::vector<int> sp_found;
    mytree->Branch("decay_x", &decay_x, "decay_x/D");
    mytree->Branch("decay_y", &decay_y, "decay_y/D");
    mytree->Branch("decay_z", &decay_z, "decay_z/D");
    mytree->Branch("truth_px", &truth_px, "truth_px/D");
    mytree->Branch("truth_py", &truth_py, "truth_py/D");
    mytree->Branch("truth_pz", &truth_pz, "truth_pz/D");
    mytree->Branch("truth_E", &truth_E, "truth_E/D");
    mytree->Branch("truth_R", &truth_R, "truth_R/D");
    mytree->Branch("truth_phi0", &truth_phi0, "truth_phi0/D");
    mytree->Branch("truth_lambda", &truth_lambda, "truth_lambda/D");
    mytree->Branch("truth_hits_x", &truth_hits_x);
    mytree->Branch("truth_hits_y", &truth_hits_y);
    mytree->Branch("truth_hits_z", &truth_hits_z);
    mytree->Branch("truth_x", &truth_x);
    mytree->Branch("truth_y", &truth_y);
    mytree->Branch("truth_z", &truth_z);
    mytree->Branch("reco_px", &reco_px, "reco_px/D");
    mytree->Branch("reco_py", &reco_py, "reco_py/D");
    mytree->Branch("reco_pz", &reco_pz, "reco_pz/D");
    mytree->Branch("reco_E", &reco_E, "reco_E/D");
    mytree->Branch("reco_R", &reco_R, "reco_R/D");
    mytree->Branch("reco_phi0", &reco_phi0, "reco_phi0/D");
    mytree->Branch("reco_lambda", &reco_lambda, "reco_lambda/D");
    mytree->Branch("reco_x", &reco_x);
    mytree->Branch("reco_y", &reco_y);
    mytree->Branch("reco_z", &reco_z);
    mytree->Branch("space_points_x", &space_points_x);
    mytree->Branch("space_points_y", &space_points_y);
    mytree->Branch("space_points_z", &space_points_z);
    mytree->Branch("track_points_x", &track_points_x);
    mytree->Branch("track_points_y", &track_points_y);
    mytree->Branch("track_points_z", &track_points_z);
    mytree->Branch("chi_square", &chi_square, "chi_square/D");
    mytree->Branch("N_track_points",&N_track_points,"N_track_points/I");
    mytree->Branch("pdgID",&pdgID,"pdgID/I");
    mytree->Branch("track_points_rowID",&track_points_rowID);
    mytree->Branch("status",&status,"status/I");
    mytree->Branch("N_shared_sp",&N_shared_sp,"N_shared_sp/I");
    mytree->Branch("d_truth_hits",&d_truth_hits);
    mytree->Branch("Is_shared_sp",&Is_shared_sp);
    mytree->Branch("dark_photon_px", &dark_photon_px, "dark_photon_px/D");
    mytree->Branch("dark_photon_py", &dark_photon_py, "dark_photon_py/D");
    mytree->Branch("dark_photon_pz", &dark_photon_pz, "dark_photon_pz/D");
    mytree->Branch("dark_photon_E", &dark_photon_E, "dark_photon_E/D");
    mytree->Branch("lep0_px", &lep0_px, "lep0_px/D");
    mytree->Branch("lep0_py", &lep0_py, "lep0_py/D");
    mytree->Branch("lep0_pz", &lep0_pz, "lep0_pz/D");
    mytree->Branch("lep0_E", &lep0_E, "lep0_E/D");
    mytree->Branch("lep1_px", &lep1_px, "lep1_px/D");
    mytree->Branch("lep1_py", &lep1_py, "lep1_py/D");
    mytree->Branch("lep1_pz", &lep1_pz, "lep1_pz/D");
    mytree->Branch("lep1_E", &lep1_E, "lep1_E/D");
    mytree->Branch("lep_plus_E", &lep_plus_E, "lep_plus_E/D");
    mytree->Branch("lep_minus_E", &lep_minus_E, "lep_minus_E/D");
    mytree->Branch("sp_found",&sp_found);

    std::vector <double> outvect(6,0);

    for (int iEntry=0; iEntry<eventTree->GetEntries(); ++iEntry) 
    //for (int iEntry=0; iEntry<50; ++iEntry) 
      {
        eventTree->GetEntry(iEntry);
        cout << "INFO  Processing event " << event->eventNumber << "\n";
        status=0;	
	int cal_stat=1, find_truth_2=0, first_flag=0, trackID;
	N_shared_sp=0;
	d_truth_hits.clear();
	Is_shared_sp.clear();

	// Require mu+/mu- pair 
        TLorentzVector fmom_plus;
        TLorentzVector fmom_minus;
	for (const FaserTrackerTruthParticle * tp : event->truthParticles) 
	  {
	    if (tp->pdgCode == 36) 
	     {
	       const TLorentzVector & fmom = tp->fourMomentum;
	       dark_photon_px = fmom.Px()/1000.0;
	       dark_photon_py = fmom.Py()/1000.0;
	       dark_photon_pz = fmom.Pz()/1000.0;
	       dark_photon_E  = fmom.E()/1000.0;
	     }
	    if (tp->pdgCode==-target_pdgID && tp->parentID==1)
	     {
	       fmom_plus = tp->fourMomentum;
	       lep_plus_E = fmom_plus.E()/1000.0;
	     }
	    if (tp->pdgCode==target_pdgID && tp->parentID==1)
	     {
	       fmom_minus = tp->fourMomentum;
	       lep_minus_E = fmom_minus.E()/1000.0;
	     }
	    if (std::abs(tp->pdgCode)==target_pdgID && tp->parentID==1) find_truth_2++;
	  }

	if (find_truth_2 != 2) continue;
	if (fmom_plus.E() > fmom_minus.E())
	 {
           lep0_px = fmom_plus.Px()/1000.0; 
           lep0_py = fmom_plus.Py()/1000.0; 
           lep0_pz = fmom_plus.Pz()/1000.0; 
           lep0_E = fmom_plus.E()/1000.0; 
           lep1_px = fmom_minus.Px()/1000.0; 
           lep1_py = fmom_minus.Py()/1000.0; 
           lep1_pz = fmom_minus.Pz()/1000.0; 
           lep1_E = fmom_minus.E()/1000.0; 
	 }
	else {
           lep1_px = fmom_plus.Px()/1000.0; 
           lep1_py = fmom_plus.Py()/1000.0; 
           lep1_pz = fmom_plus.Pz()/1000.0; 
           lep1_E = fmom_plus.E()/1000.0; 
           lep0_px = fmom_minus.Px()/1000.0; 
           lep0_py = fmom_minus.Py()/1000.0; 
           lep0_pz = fmom_minus.Pz()/1000.0; 
           lep0_E = fmom_minus.E()/1000.0; 
	}

	for (const FaserTrackerTruthParticle * tp : event->truthParticles) 
	  {
	    if (std::abs(tp->pdgCode)==target_pdgID && tp->parentID==1)
	     {

	truth_x.clear(); truth_y.clear(); truth_z.clear();
        reco_x.clear(); reco_y.clear(); reco_z.clear(); 	
        space_points_x.clear(); space_points_y.clear(); space_points_z.clear();
        track_points_x.clear(); track_points_y.clear(); track_points_z.clear();
	track_points_rowID.clear();
        
	       pdgID = tp->pdgCode;
               const TVector3 & ver = tp->vertex;
	       const TLorentzVector & fmom = tp->fourMomentum;
	       decay_x = ver.X(); 
	       decay_y = ver.Y(); 
	       decay_z = ver.Z(); 
	       for (int iz=0; iz<N_plane; iz++) 
	         {
	          if (helix(outvect,fmom.Px()/1000.0,fmom.Py()/1000.0,fmom.Pz()/1000.0,ver.X(),ver.Y(),ver.Z(),B,charge,target_z[iz],-9999) == -1) cal_stat=0;

		  //if (std::fabs(outvect[0])>128 || std::fabs(outvect[1])>128) cal_stat=0;

	          truth_x.push_back(outvect[0]);  truth_y.push_back(outvect[1]);  truth_z.push_back(outvect[2]);
                  if (iz==0) {truth_px=outvect[3]; truth_py=outvect[4]; truth_pz=outvect[5]; truth_E=fmom.E()/1000.0;}
	         }
	       int truth_charge=-1;
	       //if (pdgID<0) truth_charge=1;
	       if (pdgID>0) truth_charge=1;   // It is WRONG !!!!
	       charge=truth_charge;
	       double truth_p_perp = sqrt(truth_px*truth_px + truth_pz*truth_pz);  // The magnet field point to positive y axis
	       double truth_sin_lambda = truth_py/sqrt(truth_p_perp*truth_p_perp + truth_py*truth_py);
	       truth_lambda = std::asin(truth_sin_lambda); 
	       truth_R = 1000*truth_p_perp/(0.3*B);  // R(mm), p(GeV), B(T)
	       double truth_vRx = -truth_charge*truth_pz/truth_p_perp;  // vR is a norm vecter pointing from intial position to the center of circle
	       double truth_vRz = truth_charge*truth_px/truth_p_perp;
	       if (-truth_vRx > 0) truth_phi0 = std::asin(-truth_vRz);
                 else if (-truth_vRz >0) truth_phi0 = TMath::Pi() - std::asin(-truth_vRz);
                        else truth_phi0 =  -TMath::Pi() - std::asin(-truth_vRz);

	if (!cal_stat) { status=-1;}

	trackID = tp->trackID;  
	truth_hits_x.clear(); truth_hits_y.clear(); truth_hits_z.clear(); 
	sp_found.clear(); 
	for (int iz=0; iz<N_plane; iz++) 
	  {
            int flag=0;
	    double total_E=0;
	    truth_hits_x.push_back(0);
	    truth_hits_y.push_back(0);
	    truth_hits_z.push_back(0);
        for (const FaserTrackerTruthHit * th : event->truthHits) 
	  {
	    if (th->trackID == trackID)
	     {
               const TVector3 & thpos = th->globalPos;
	       if (std::fabs(thpos.Z()-target_z[iz]) < 20)
	        {
		  truth_hits_x[iz] += th->energy * thpos.X();
		  truth_hits_y[iz] += th->energy * thpos.Y();
		  truth_hits_z[iz] += th->energy * thpos.Z();
		  total_E += th->energy; 
		}
	     }
	  }
	     truth_hits_x[iz] /= total_E;
	     truth_hits_y[iz] /= total_E;
	     truth_hits_z[iz] /= total_E;
	
	    if (total_E>0) {
               for (const FaserTrackerSpacePoint * sp : event->spacePoints) 
	         {
                   const TVector3 & pos = sp->globalPos;
	           if (std::fabs(pos.Z()-target_z[iz]) < 20) 
	            {
                   for (uint icl : sp->analogClusterIndices) {
		      FaserTrackerCluster * cl = event->analogClusters[icl];
                      for (uint idigit : cl->digitIndices) {
			 FaserTrackerDigit * digit = event->digits[idigit];
                         for (uint iHit : digit->truthHitIndices) 
			   {
                             FaserTrackerTruthHit * th = event->truthHits[iHit];
			     if (th->trackID == trackID) flag=1;
			   }
		      }
		   }
		    }
		   
	         }
	      if (flag==1) sp_found.push_back(1);
	        else sp_found.push_back(0);
	    }
	    else {
	      sp_found.push_back(-1);
	    }

	  }

	mytree->Fill();
	     }
	  }
        
       }

    outf->cd();
    mytree->Write();
    outf->Close();
    //delete mytree;
    delete outf;
}

