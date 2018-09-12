#include "FaserTracker/TrackCandidate.hh"
#include "FaserTracker/common_includes.hh"


namespace FaserTracker {

//------------------------------------------------------------------------------

//void TrackCandidate::addSpacePoint(const SpacePoint & spacePoint) {

//    if (eventNumber < 0) {
//        eventNumber = spacePoint.eventNumber;
//    }

//    if (eventNumber > -1 && spacePoint.eventNumber != eventNumber) {
//        cout << "WARNING  TrackCandidate::addSpacePoint\n"
//             << "         Adding space point with event number different from the others\n";
//    }

//    spacePoints->push_back(spacePoint);

//}

//------------------------------------------------------------------------------

bool TrackCandidate::isValid() const {

    //bool hitPlanes [N_PLANES] = {false};

    //for (const DigiCluster & cluster : *digiClusters) {
    //    hitPlanes[cluster.plane] = true;
    //}

    //for (bool hitPresent : hitPlanes) {
    //    if (!hitPresent) return false;
    //}

    return true;

}

//------------------------------------------------------------------------------

//void TrackCandidate::dumpSpacePoints() const {

//    cout << "INFO  FaserTracker::TrackCandidate::dumpSpacePoints\n"
//         << "      Dumping space points in track candidate:\n";

//    for (const DigiCluster & sp : *spacePoints) {
//        spacePoint.print();
//    }

//}

//------------------------------------------------------------------------------

}

