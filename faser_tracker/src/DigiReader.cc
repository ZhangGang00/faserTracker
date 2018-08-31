#include "FaserTracker/common_includes.hh"
#include "FaserTracker/DigiReader.hh"
#include "TChain.h"

namespace FaserTracker {

    DigiReader::DigiReader(TChain & inputChain_) :
        inputChain {&inputChain_}
    {
        inputChain->SetBranchAddress("digi_plane" , &p_digiPlane);
        inputChain->SetBranchAddress("digi_module", &p_digiModule);
        inputChain->SetBranchAddress("digi_sensor", &p_digiSensor);
        inputChain->SetBranchAddress("digi_row"   , &p_digiRow);
        inputChain->SetBranchAddress("digi_strip" , &p_digiStrip);
        inputChain->SetBranchAddress("digi_charge", &p_digiCharge);
    
        inputChain->SetBranchAddress("truth_plane"    , &p_truthPlane);
        inputChain->SetBranchAddress("truth_module"   , &p_truthModule);
        inputChain->SetBranchAddress("truth_sensor"   , &p_truthSensor);
        inputChain->SetBranchAddress("truth_row"      , &p_truthRow);
        inputChain->SetBranchAddress("truth_strip"    , &p_truthStrip);
        inputChain->SetBranchAddress("truth_track"    , &p_truthTrack);
        inputChain->SetBranchAddress("truth_global_x" , &p_truthGlobalX);
        inputChain->SetBranchAddress("truth_global_y" , &p_truthGlobalY);
        inputChain->SetBranchAddress("truth_global_z" , &p_truthGlobalZ);
        inputChain->SetBranchAddress("truth_local_x"  , &p_truthLocalX);
        inputChain->SetBranchAddress("truth_local_y"  , &p_truthLocalY);
        inputChain->SetBranchAddress("truth_local_z"  , &p_truthLocalZ);
        inputChain->SetBranchAddress("truth_vertex_x" , &p_truthVertexX);
        inputChain->SetBranchAddress("truth_vertex_y" , &p_truthVertexY);
        inputChain->SetBranchAddress("truth_vertex_z" , &p_truthVertexZ);
        inputChain->SetBranchAddress("truth_vertex_px", &p_truthVertexPx);
        inputChain->SetBranchAddress("truth_vertex_py", &p_truthVertexPy);
        inputChain->SetBranchAddress("truth_vertex_pz", &p_truthVertexPz);
        inputChain->SetBranchAddress("truth_vertex_ke", &p_truthVertexKE);
    }

    ////////////////////////////////////////////////////////////////////////////////

    void DigiReader::getEntry(long entry) {

        if (inputChain == nullptr) {
            cout << "ERROR  DigiReader::getEntry\n"
                 << "       Unable to retrieve entry " << entry
                 <<        " ... the input chain is null!\n"
                 << "\n";

            throw runtime_error {"null chain"};
        }

        inputChain->GetEntry(entry);
        eventNumber = entry;

    }

    ////////////////////////////////////////////////////////////////////////////////

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

        auto digits_ = make_shared<vector<Digit>>();

        for (uint i=0; i<truthGlobalX.size(); ++i) {
            TVector3 posVec = {truthGlobalX[i], truthGlobalY[i], truthGlobalZ[i]};

            // Using charge = 1.0 for truth --> TODO: check if all weighted the same
            digits_->push_back({eventNumber, truthPlane[i], 1.0, posVec, truthTrack[i]});
        }

        _cachedDigits = digits_;

        return digits_;

    }
}

