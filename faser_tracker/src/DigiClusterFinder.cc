#include "FaserTracker/common_includes.hh"
#include "FaserTracker/DigiClusterFinder.hh"


namespace FaserTracker {

    std::shared_ptr<std::vector<DigiCluster>>
        DigiClusterFinder::findDigitClusters(const DigiReader & digiReader)
    {

        vector<uint> planeDigits [N_PLANES];

        shared_ptr<vector<Digit>> digits = digiReader.digits();

        for (uint iDigit = 0; iDigit < digits->size(); ++iDigit) {

            const Digit & digit = digits->at(iDigit);

            if (digit.plane < 0 || digit.plane >= N_PLANES) {
                cout << "ERROR  FaserTracker::DigiClusterFinding::findDigitClusters\n"
                     << "       Illegal plane number: " << digit.plane << "\n"
                     << "       (must range from 0 to " << N_PLANES-1 << ")\n";

                throw runtime_error {"illegal FASER plane number"};
            }

            planeDigits[digit.plane].push_back(iDigit);
        }

        // will return `clusters`
        auto clusters = make_shared<vector<DigiCluster>>();

        // The following is a bit inefficient, but should do for the relatively
        // few digits per event in FASER.
        for (int iPlane = 0; iPlane < N_PLANES; ++iPlane) {

            for (uint iDigit : planeDigits[iPlane]) {
                const Digit & digitI = digits->at(iDigit);
                const TVector3 & posI = *digitI.globalPosition();

                bool clusterMatched = false;
                for (DigiCluster & cluster : *clusters) {
                    for (Digit & digit : *cluster.digits) {
                        if ((posI - *digit.globalPosition()).Mag() < distanceTolerance) {
                            cluster.addDigit(digitI);
                            clusterMatched = true;
                            break;
                        }
                    }
                    if (clusterMatched) break;
                }

                if (!clusterMatched) {
                    // Start new cluster on plane `iPlane`
                    clusters->push_back({iPlane});
                    clusters->back().addDigit(digitI);
                }
            }
        }

        return clusters;

    }

}

