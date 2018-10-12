#include "FaserTracker/TrackCandidate.hh"
#include "FaserTracker/common_includes.hh"


namespace FaserTracker {

//------------------------------------------------------------------------------

void TrackCandidate::addSpacePoint(const SpacePoint & sp) {

    //if (eventNumber < 0) {
    //    eventNumber = sp.eventNumber;
    //}

    //if (eventNumber > -1 && sp.eventNumber != eventNumber) {
    //    cout << "WARNING  TrackCandidate::addSpacePoint\n"
    //         << "         Adding space point with event number different from the others\n";
    //}

    spacePoints.push_back(sp);

}

//------------------------------------------------------------------------------

bool TrackCandidate::isValid() const {

    bool hitPlanes [N_PLANES] = {false};

    for (const SpacePoint & sp : spacePoints) {
        hitPlanes[sp.plane] = true;
    }

    for (bool hitPresent : hitPlanes) {
        if (!hitPresent) return false;
    }

    return true;

}

//------------------------------------------------------------------------------

void TrackCandidate::dumpSpacePoints() const {

    cout << "INFO  Dumping space points in track candidate\n\n";

    for (const SpacePoint & sp : spacePoints) {
        sp.print();
    }

}

//------------------------------------------------------------------------------

} // namespace

