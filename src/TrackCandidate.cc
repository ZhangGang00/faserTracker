#include "FaserTracker/common_includes.hh"
#include "FaserTracker/TrackCandidate.hh"
#include "FaserTracker/DigiCluster.hh"


namespace FaserTracker {

    void TrackCandidate::addCluster(const DigiCluster & cluster) {

        if (eventNumber < 0) {
            eventNumber = cluster.eventNumber;
        }

        if (eventNumber > -1 && cluster.eventNumber != eventNumber) {
            cout << "WARNING  TrackCandidate::addCluster\n"
                 << "         Adding cluster with event number different from the others\n";
        }

        digiClusters->push_back(cluster);

    }

    ////////////////////////////////////////////////////////////////////////////////

    bool TrackCandidate::isValid() const {

        bool hitPlanes [N_PLANES] = {false};

        for (const DigiCluster & cluster : *digiClusters) {
            hitPlanes[cluster.plane] = true;
        }

        for (bool hitPresent : hitPlanes) {
            if (!hitPresent) return false;
        }

        return true;

    }

    ////////////////////////////////////////////////////////////////////////////////

    void TrackCandidate::dumpDigitClusters() const {

        cout << "INFO  Dumping digit clusters in track candidate\n\n";

        for (const DigiCluster & cluster : *digiClusters) {
            cluster.print();
        }

    }

}

