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

static std::vector<TVector3> selected_track_point;
static const double B=0.6;
static const int N_plane=9; 
//static double target_z[8]={0, 50, 909.04, 959.04, 1009.04, 1059.04, 1942.02, 1992.02};
double target_z[9]={0, 50, 100, 938.03, 988.03, 1038.03, 1892.02, 1942.02, 1992.02};   //ITK
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


    TFile *outf = new TFile("/afs/cern.ch/user/g/gang/FASER/faser_tracker/sp_separation_output.root","recreate");
    TTree *mytree = new TTree("sp_separation","");
    std::vector<double> plus_truth_hits_x;
    std::vector<double> plus_truth_hits_y;
    std::vector<double> plus_truth_hits_z;
    std::vector<double> plus_space_points_x;
    std::vector<double> plus_space_points_y;
    std::vector<double> plus_space_points_z;
    std::vector<double> plus_truth_hits_fraction;
    std::vector<double> minus_truth_hits_x;
    std::vector<double> minus_truth_hits_y;
    std::vector<double> minus_truth_hits_z;
    std::vector<double> minus_space_points_x;
    std::vector<double> minus_space_points_y;
    std::vector<double> minus_space_points_z;
    std::vector<double> minus_truth_hits_fraction;
    std::vector<int> Is_shared_sp;
    std::vector<double> d_truth_hits;
    std::vector<double> d_space_points;
    int N_shared_sp;
    double dark_photon_px,dark_photon_py,dark_photon_pz,dark_photon_E,lep_plus_px,lep_plus_py,lep_plus_pz,lep_plus_E,lep_minus_px,lep_minus_py,lep_minus_pz,lep_minus_E;
    mytree->Branch("plus_truth_hits_x", &plus_truth_hits_x);
    mytree->Branch("plus_truth_hits_y", &plus_truth_hits_y);
    mytree->Branch("plus_truth_hits_z", &plus_truth_hits_z);
    mytree->Branch("plus_space_points_x", &plus_space_points_x);
    mytree->Branch("plus_space_points_y", &plus_space_points_y);
    mytree->Branch("plus_space_points_z", &plus_space_points_z);
    mytree->Branch("plus_truth_hits_fraction", &plus_truth_hits_fraction);
    mytree->Branch("minus_truth_hits_x", &minus_truth_hits_x);
    mytree->Branch("minus_truth_hits_y", &minus_truth_hits_y);
    mytree->Branch("minus_truth_hits_z", &minus_truth_hits_z);
    mytree->Branch("minus_space_points_x", &minus_space_points_x);
    mytree->Branch("minus_space_points_y", &minus_space_points_y);
    mytree->Branch("minus_space_points_z", &minus_space_points_z);
    mytree->Branch("minus_truth_hits_fraction", &minus_truth_hits_fraction);
    mytree->Branch("d_truth_hits",&d_truth_hits);
    mytree->Branch("d_space_points",&d_space_points);
    mytree->Branch("Is_shared_sp",&Is_shared_sp);
    mytree->Branch("N_shared_sp",&N_shared_sp,"N_shared_sp/I");
    mytree->Branch("lep_plus_px",&lep_plus_px,"lep_plus_px/D");
    mytree->Branch("lep_plus_py",&lep_plus_py,"lep_plus_py/D");
    mytree->Branch("lep_plus_pz",&lep_plus_pz,"lep_plus_pz/D");
    mytree->Branch("lep_plus_E",&lep_plus_E,"lep_plus_E/D");
    mytree->Branch("lep_minus_px",&lep_minus_px,"lep_minus_px/D");
    mytree->Branch("lep_minus_py",&lep_minus_py,"lep_minus_py/D");
    mytree->Branch("lep_minus_pz",&lep_minus_pz,"lep_minus_pz/D");
    mytree->Branch("lep_minus_E",&lep_minus_E,"lep_minus_E/D");
    mytree->Branch("dark_photon_px", &dark_photon_px, "dark_photon_px/D");
    mytree->Branch("dark_photon_py", &dark_photon_py, "dark_photon_py/D");
    mytree->Branch("dark_photon_pz", &dark_photon_pz, "dark_photon_pz/D");
    mytree->Branch("dark_photon_E", &dark_photon_E, "dark_photon_E/D");

    FaserTrackerEvent * event = nullptr;
    eventTree->SetBranchAddress("events", &event);
    double hit[N_plane][1000]={0}; //hit[i][0] is the hits in corresponding layer, hit[i][j>0] are coordinates
    double hit_n1[N_plane][1000]={0};
    double hit_n2[N_plane][1000]={0};
    double hit_frac1[N_plane][1000]={0};
    double hit_frac2[N_plane][1000]={0};
    double hit_pos1[N_plane][1000]={0};
    double hit_pos2[N_plane][1000]={0};
    int N_hit_plane=0;
    std::vector <double> outvect(6,0);

    int trackID_plus, trackID_minus;
    for (int iEntry=0; iEntry<eventTree->GetEntries(); ++iEntry) 
    //for (int iEntry=0; iEntry<50; ++iEntry) 
      {
        eventTree->GetEntry(iEntry);
        cout << "INFO  Processing event " << event->eventNumber << "\n";
#if 1	
	int cal_stat=1, find_truth_2=0;

	plus_truth_hits_x.clear(); plus_truth_hits_y.clear(); plus_truth_hits_z.clear(); 
	plus_space_points_x.clear(); plus_space_points_y.clear(); plus_space_points_z.clear();  
	minus_truth_hits_x.clear(); minus_truth_hits_y.clear(); minus_truth_hits_z.clear(); 
	minus_space_points_x.clear(); minus_space_points_y.clear(); minus_space_points_z.clear();  
        d_truth_hits.clear(); d_space_points.clear(); Is_shared_sp.clear();
	plus_truth_hits_fraction.clear(); minus_truth_hits_fraction.clear(); 
        N_shared_sp=0;	
        
	// Require mu+/mu- pair 
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
	    if (std::abs(tp->pdgCode)==target_pdgID && tp->parentID==1) 
	     {
	       find_truth_2++;
	       const TLorentzVector & fmom = tp->fourMomentum;
	       if (tp->pdgCode > 0) 
	        {
		  trackID_minus = tp->trackID;
		  lep_minus_px = fmom.Px()/1000.0;
		  lep_minus_py = fmom.Py()/1000.0;
		  lep_minus_pz = fmom.Pz()/1000.0;
		  lep_minus_E  = fmom.E()/1000.0;
		}
	       if (tp->pdgCode < 0) 
	        {
		  trackID_plus = tp->trackID;
		  lep_plus_px = fmom.Px()/1000.0;
		  lep_plus_py = fmom.Py()/1000.0;
		  lep_plus_pz = fmom.Pz()/1000.0;
		  lep_plus_E  = fmom.E()/1000.0;
		}
	     }
	  }
	if (find_truth_2 != 2) continue;

	N_hit_plane=0;
	for (int i=0; i<N_plane; i++) 
	  { 
	    for (int j=0; j<1000; j++) 
	      {
		 hit[i][j]=0; 
		 hit_pos1[i][j]=0; 
		 hit_pos2[i][j]=0; 
                 hit_n1[i][j]=0;
                 hit_n2[i][j]=0;
                 hit_frac1[i][j]=0;
                 hit_frac2[i][j]=0;
	      }
	  }

        for (const FaserTrackerSpacePoint * sp : event->spacePoints) 
	{
            const TVector3 & pos = sp->globalPos;

	    //if (std::fabs(pos.X())>100 || std::fabs(pos.Y())>100) continue;
            cout << "      Read space point with global position (" << pos.X() << "," << pos.Y() << "," << pos.Z() << ")\n";

	    for (int i=0; i<N_plane; i++) { 
	       if (std::fabs(pos.Z()-target_z[i]) < 20) 
	        {
		   hit[i][3*(int)hit[i][0]+1]=pos.X();
		   hit[i][3*(int)hit[i][0]+2]=pos.Y();
		   hit[i][3*(int)hit[i][0]+3]=pos.Z();
		   if (hit[i][0]==0) N_hit_plane++;
		   int N_th=0; 
		   double total_E1=0, total_E2=0;
                   for (uint icl : sp->analogClusterIndices) {
		      FaserTrackerCluster * cl = event->analogClusters[icl];
                      for (uint idigit : cl->digitIndices) {
			 FaserTrackerDigit * digit = event->digits[idigit];
                         for (uint iHit : digit->truthHitIndices) 
                         //for (uint iHit : sp->truthHitIndices) 
			   {
                             FaserTrackerTruthHit * th = event->truthHits[iHit];
                             const TVector3 & thpos = th->globalPos;
                             cout<<"truth hits:   trackID = "<<th->trackID<<"  coordinate = "<<thpos.X()<<"  "<<thpos.Y()<<"  "<<thpos.Z()<<"\n";

			     if (th->trackID==trackID_minus)
			      {
				hit_n1[i][(int)hit[i][0]]++;
				hit_pos1[i][3*(int)hit[i][0]+1] += th->energy * thpos.X(); 
				hit_pos1[i][3*(int)hit[i][0]+2] += th->energy * thpos.Y(); 
				hit_pos1[i][3*(int)hit[i][0]+3] += th->energy * thpos.Z(); 
				N_th++;
				total_E1 += th->energy; 
			      }
			     if (th->trackID==trackID_plus)
			      {
				hit_n2[i][(int)hit[i][0]]++;
				hit_pos2[i][3*(int)hit[i][0]+1] += th->energy * thpos.X(); 
				hit_pos2[i][3*(int)hit[i][0]+2] += th->energy * thpos.Y(); 
				hit_pos2[i][3*(int)hit[i][0]+3] += th->energy * thpos.Z(); 
				N_th++;
				total_E2 += th->energy; 
			      }
	                   }
	              }
		   }
		 
		   if (N_th > 0) { 
                   hit_frac1[i][(int)hit[i][0]] = hit_n1[i][(int)hit[i][0]]/(double)N_th;
                   hit_frac2[i][(int)hit[i][0]] = hit_n2[i][(int)hit[i][0]]/(double)N_th;
		   } else {
                   hit_frac1[i][(int)hit[i][0]] = -9999;
                   hit_frac2[i][(int)hit[i][0]] = -9999;
		   }
		   if (total_E1 > 0) {
		   hit_pos1[i][3*(int)hit[i][0]+1] /= total_E1; 
		   hit_pos1[i][3*(int)hit[i][0]+2] /= total_E1; 
		   hit_pos1[i][3*(int)hit[i][0]+3] /= total_E1; 
		   } else {
		   hit_pos1[i][3*(int)hit[i][0]+1] = -9999; 
		   hit_pos1[i][3*(int)hit[i][0]+2] = -9999; 
		   hit_pos1[i][3*(int)hit[i][0]+3] = -9999; 
		   }
		   if (total_E2 > 0) {
		   hit_pos2[i][3*(int)hit[i][0]+1] /= total_E2; 
		   hit_pos2[i][3*(int)hit[i][0]+2] /= total_E2; 
		   hit_pos2[i][3*(int)hit[i][0]+3] /= total_E2; 
		   } else {
		   hit_pos2[i][3*(int)hit[i][0]+1] = -9999; 
		   hit_pos2[i][3*(int)hit[i][0]+2] = -9999; 
		   hit_pos2[i][3*(int)hit[i][0]+3] = -9999; 
		   }
		   hit[i][0]++;
		} 
	    }
        }
    
        //if (N_hit_plane<4) continue;  // discard the tracks with less than 4 space points

        for (int i=0; i<N_plane; i++)  
	  {
	     if (hit[i][0] > 0) {
		int temp1=0, temp2=0;
		for (int j=1; j<hit[i][0]; j++) {
                   if ((hit_n1[i][j] > hit_n1[i][temp1]) || (hit_n1[i][j]==hit_n1[i][temp1] && hit_frac1[i][j] > hit_frac1[i][temp1])) 
		    {
		     temp1=j;
		    }
                   if ((hit_n2[i][j] > hit_n2[i][temp2]) || (hit_n2[i][j]==hit_n2[i][temp2] && hit_frac2[i][j] > hit_frac2[i][temp2])) 
		    {
		     temp2=j;
		    }
		} 
#if 1
		for (int j=0; j<hit[i][0]; j++) { cout<<"plane = "<<i<<"   (x,y,z) = ("<<hit[i][3*j+1]<<", "<<hit[i][3*j+2]<<", "<<hit[i][3*j+3]<<")"<<"  hit_n1 = "<<hit_n1[i][j]<<"  "<<"hit_n2 = "<<hit_n2[i][j]<<"     hit_frac1 = "<<hit_frac1[i][j]<<"  "<<"hit_frac2 = "<<hit_frac2[i][j]<<"\n"; }
		cout<<"temp1 = "<<temp1<<"   temp2 = "<<temp2<<"\n";
		cout<<"hit_n1 = "<<hit_n1[i][temp1]<<"     "<<"hit_n2 = "<<hit_n2[i][temp2]<<"\n";
		cout<<"hit_frac1 = "<<hit_frac1[i][temp1]<<"      "<<"hit_frac2 = "<<hit_frac2[i][temp2]<<"\n";
		cout<<"hit_pos1 = ("<<hit_pos1[i][3*temp1+1]<<", "<<hit_pos1[i][3*temp1+2]<<", "<<hit_pos1[i][3*temp1+3]<<")     "<<"hit_pos2 = ("<<hit_pos2[i][3*temp2+1]<<", "<<hit_pos2[i][3*temp2+2]<<", "<<hit_pos2[i][3*temp2+3]<<")\n";
		cout<<"distance = "<<sqrt(pow(hit_pos1[i][3*temp1+1]-hit_pos2[i][3*temp2+1],2)+pow(hit_pos1[i][3*temp1+2]-hit_pos2[i][3*temp2+2],2))<<"\n\n";
#endif
	       if (hit_n1[i][temp1]>0) {
               minus_truth_hits_x.push_back(hit_pos1[i][3*temp1+1]);
               minus_truth_hits_y.push_back(hit_pos1[i][3*temp1+2]);
               minus_truth_hits_z.push_back(hit_pos1[i][3*temp1+3]);
               minus_space_points_x.push_back(hit[i][3*temp1+1]);
               minus_space_points_y.push_back(hit[i][3*temp1+2]);
               minus_space_points_z.push_back(hit[i][3*temp1+3]);
               minus_truth_hits_fraction.push_back(hit_frac1[i][temp1]);
	       } else {
               minus_truth_hits_x.push_back(-9999);
               minus_truth_hits_y.push_back(-9999);
               minus_truth_hits_z.push_back(-9999);
               minus_space_points_x.push_back(-9999);
               minus_space_points_y.push_back(-9999);
               minus_space_points_z.push_back(-9999);
               minus_truth_hits_fraction.push_back(-9999);
	       }
	       if (hit_n2[i][temp2]>0) {
               plus_truth_hits_x.push_back(hit_pos2[i][3*temp2+1]);
               plus_truth_hits_y.push_back(hit_pos2[i][3*temp2+2]);
               plus_truth_hits_z.push_back(hit_pos2[i][3*temp2+3]);
               plus_space_points_x.push_back(hit[i][3*temp2+1]);
               plus_space_points_y.push_back(hit[i][3*temp2+2]);
               plus_space_points_z.push_back(hit[i][3*temp2+3]);
               plus_truth_hits_fraction.push_back(hit_frac2[i][temp2]);
	       } else {
               plus_truth_hits_x.push_back(-9999);
               plus_truth_hits_y.push_back(-9999);
               plus_truth_hits_z.push_back(-9999);
               plus_space_points_x.push_back(-9999);
               plus_space_points_y.push_back(-9999);
               plus_space_points_z.push_back(-9999);
               plus_truth_hits_fraction.push_back(-9999);
	       }
	       if (hit_n1[i][temp1]>0 && hit_n2[i][temp2]>0) {
	         if (temp1 == temp2) 
		  { 
	            d_truth_hits.push_back(sqrt(pow(hit_pos1[i][3*temp1+1]-hit_pos2[i][3*temp2+1],2)+pow(hit_pos1[i][3*temp1+2]-hit_pos2[i][3*temp2+2],2)));	   
	            d_space_points.push_back(sqrt(pow(hit[i][3*temp1+1]-hit[i][3*temp2+1],2)+pow(hit[i][3*temp1+2]-hit[i][3*temp2+2],2)));	   
		    Is_shared_sp.push_back(1); 
		    N_shared_sp++; 
		  }
	           else {
	              d_truth_hits.push_back(sqrt(pow(hit_pos1[i][3*temp1+1]-hit_pos2[i][3*temp2+1],2)+pow(hit_pos1[i][3*temp1+2]-hit_pos2[i][3*temp2+2],2)));	   
	              d_space_points.push_back(sqrt(pow(hit[i][3*temp1+1]-hit[i][3*temp2+1],2)+pow(hit[i][3*temp1+2]-hit[i][3*temp2+2],2)));	   
		      Is_shared_sp.push_back(0);
		   }
	       }
	       else {
	         d_truth_hits.push_back(-9999);
	         d_space_points.push_back(-9999);
	         Is_shared_sp.push_back(-1);
	       } 
	     }
	   else {
               minus_truth_hits_x.push_back(-9999);
               minus_truth_hits_y.push_back(-9999);
               minus_truth_hits_z.push_back(-9999);
               minus_space_points_x.push_back(-9999);
               minus_space_points_y.push_back(-9999);
               minus_space_points_z.push_back(-9999);
               minus_truth_hits_fraction.push_back(-9999);
               plus_truth_hits_x.push_back(-9999);
               plus_truth_hits_y.push_back(-9999);
               plus_truth_hits_z.push_back(-9999);
               plus_space_points_x.push_back(-9999);
               plus_space_points_y.push_back(-9999);
               plus_space_points_z.push_back(-9999);
               plus_truth_hits_fraction.push_back(-9999);
	     d_truth_hits.push_back(-9999);
	     d_space_points.push_back(-9999);
	     Is_shared_sp.push_back(-1);
	   }
	  }

	mytree->Fill();
#endif
    }

    outf->cd();
    mytree->Write();
    outf->Close();
    //delete mytree;
    delete outf;
}

