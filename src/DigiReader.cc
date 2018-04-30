#include "FaserTracker/DigiReader.hh"
#include "TChain.h"

namespace FaserTracker {

    DigiReader::DigiReader(TChain & inputTree) {
        inputTree.SetBranchAddress("digi_plane" , &p_digiPlane);
        inputTree.SetBranchAddress("digi_module", &p_digiModule);
        inputTree.SetBranchAddress("digi_sensor", &p_digiSensor);
        inputTree.SetBranchAddress("digi_row"   , &p_digiRow);
        inputTree.SetBranchAddress("digi_strip" , &p_digiStrip);
        inputTree.SetBranchAddress("digi_charge", &p_digiCharge);
    
        inputTree.SetBranchAddress("truth_plane"    , &p_truthPlane);
        inputTree.SetBranchAddress("truth_module"   , &p_truthModule);
        inputTree.SetBranchAddress("truth_sensor"   , &p_truthSensor);
        inputTree.SetBranchAddress("truth_row"      , &p_truthRow);
        inputTree.SetBranchAddress("truth_strip"    , &p_truthStrip);
        inputTree.SetBranchAddress("truth_track"    , &p_truthTrack);
        inputTree.SetBranchAddress("truth_global_x" , &p_truthGlobalX);
        inputTree.SetBranchAddress("truth_global_y" , &p_truthGlobalY);
        inputTree.SetBranchAddress("truth_global_z" , &p_truthGlobalZ);
        inputTree.SetBranchAddress("truth_local_x"  , &p_truthLocalX);
        inputTree.SetBranchAddress("truth_local_y"  , &p_truthLocalY);
        inputTree.SetBranchAddress("truth_local_z"  , &p_truthLocalZ);
        inputTree.SetBranchAddress("truth_vertex_x" , &p_truthVertexX);
        inputTree.SetBranchAddress("truth_vertex_y" , &p_truthVertexY);
        inputTree.SetBranchAddress("truth_vertex_z" , &p_truthVertexZ);
        inputTree.SetBranchAddress("truth_vertex_px", &p_truthVertexPx);
        inputTree.SetBranchAddress("truth_vertex_py", &p_truthVertexPy);
        inputTree.SetBranchAddress("truth_vertex_pz", &p_truthVertexPz);
        inputTree.SetBranchAddress("truth_vertex_ke", &p_truthVertexKE);
    }

}

