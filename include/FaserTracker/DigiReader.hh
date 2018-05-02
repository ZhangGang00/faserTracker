#pragma once

#include <memory>
#include <utility>
#include <vector>
#include "TVector3.h"

class TChain;


namespace FaserTracker {

    struct DigiReader {

        std::vector<int>     digiPlane;
        std::vector<int>     digiModule;
        std::vector<int>     digiSensor;
        std::vector<int>     digiRow;
        std::vector<int>     digiStrip;
        std::vector<double>  digiCharge;

        std::vector<int>     truthPlane;
        std::vector<int>     truthModule;
        std::vector<int>     truthSensor;
        std::vector<int>     truthRow;
        std::vector<int>     truthStrip;
        std::vector<int>     truthTrack;
        std::vector<double>  truthGlobalX;
        std::vector<double>  truthGlobalY;
        std::vector<double>  truthGlobalZ;
        std::vector<double>  truthLocalX;
        std::vector<double>  truthLocalY;
        std::vector<double>  truthLocalZ;
        std::vector<double>  truthVertexX;
        std::vector<double>  truthVertexY;
        std::vector<double>  truthVertexZ;
        std::vector<double>  truthVertexPx;
        std::vector<double>  truthVertexPy;
        std::vector<double>  truthVertexPz;
        std::vector<double>  truthVertexKE;

        std::vector<int> *    p_digiPlane  = &digiPlane;
        std::vector<int> *    p_digiModule = &digiModule;
        std::vector<int> *    p_digiSensor = &digiSensor;
        std::vector<int> *    p_digiRow    = &digiRow;
        std::vector<int> *    p_digiStrip  = &digiStrip;
        std::vector<double> * p_digiCharge = &digiCharge;

        std::vector<int> *    p_truthPlane    = &truthPlane;
        std::vector<int> *    p_truthModule   = &truthModule;
        std::vector<int> *    p_truthSensor   = &truthSensor;
        std::vector<int> *    p_truthRow      = &truthRow;
        std::vector<int> *    p_truthStrip    = &truthStrip;
        std::vector<int> *    p_truthTrack    = &truthTrack;
        std::vector<double> * p_truthGlobalX  = &truthGlobalX;
        std::vector<double> * p_truthGlobalY  = &truthGlobalY;
        std::vector<double> * p_truthGlobalZ  = &truthGlobalZ;
        std::vector<double> * p_truthLocalX   = &truthLocalX;
        std::vector<double> * p_truthLocalY   = &truthLocalY;
        std::vector<double> * p_truthLocalZ   = &truthLocalZ;
        std::vector<double> * p_truthVertexX  = &truthVertexX;
        std::vector<double> * p_truthVertexY  = &truthVertexY;
        std::vector<double> * p_truthVertexZ  = &truthVertexZ;
        std::vector<double> * p_truthVertexPx = &truthVertexPx;
        std::vector<double> * p_truthVertexPy = &truthVertexPy;
        std::vector<double> * p_truthVertexPz = &truthVertexPz;
        std::vector<double> * p_truthVertexKE = &truthVertexKE;

        DigiReader(TChain & inputTree);

        std::shared_ptr<std::vector<std::pair<TVector3, int>>> truthGlobalPositions() const;


    private:

        mutable std::shared_ptr<std::vector<std::pair<TVector3, int>>> _cachedTruthGlobalPositions = nullptr;

    };

}

