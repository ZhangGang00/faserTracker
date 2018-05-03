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

    shared_ptr<vector<Digit>> DigiReader::digits() const {

        if (_cachedDigits != nullptr) {
            return _cachedDigits;
        }

        uint nDigits = truthTrack.size();
        if (truthPlane.size()   != nDigits ||
            truthGlobalX.size() != nDigits ||
            truthGlobalY.size() != nDigits ||
            truthGlobalZ.size() != nDigits)
        {
            cout << "ERROR  DigiReader::truthGlobalPositions\n"
                 << "       truthGlobalX, truthGlobalY, truthGlobalZ must have the same size!\n"
                 << "\n";
            throw runtime_error {"invalid global coordinate lists stored in `DigiReader`"};
        }

        auto digits = make_shared<vector<Digit>>();

        for (int i=0; i<truthGlobalX.size(); ++i) {
            TVector3 posVec = {truthGlobalX[i], truthGlobalY[i], truthGlobalZ[i]};

            // Using charge = 1.0 for truth --> TODO: check if all weighted the same
            digits->push_back({truthPlane[i], 1.0, posVec, truthTrack[i]});
        }

        _cachedDigits = digits;

        return digits;

    }
}

