#include "FaserTracker/common_includes.hh"
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

    /**
     * Extracts truth global positions as `vector` of `TVector3`s.
     * TODO: Store `Digit`s in faserMC output to avoid needing to do this.
     */
    shared_ptr<vector<pair<TVector3, int>>> DigiReader::truthGlobalPositions() const {

        if (_cachedTruthGlobalPositions != nullptr) {
            return _cachedTruthGlobalPositions;
        }

        if (truthGlobalX.size() != truthGlobalY.size() ||
            truthGlobalX.size() != truthGlobalZ.size() ||
            truthGlobalX.size() != truthTrack.size())
        {
            cout << "ERROR  DigiReader::truthGlobalPositions\n"
                 << "       truthGlobalX, truthGlobalY, truthGlobalZ must have the same size!\n"
                 << "\n";
            throw runtime_error {"invalid global coordinate lists stored in `DigiReader`"};
        }

        auto positions = make_shared<vector<pair<TVector3, int>>>();

        for (int i=0; i<truthGlobalX.size(); ++i) {
            TVector3 posVec = {truthGlobalX[i], truthGlobalY[i], truthGlobalZ[i]};
            int trackId = truthTrack[i];
            positions->push_back({posVec, trackId});
        }

        _cachedTruthGlobalPositions = positions;

        return positions;

    }
}

