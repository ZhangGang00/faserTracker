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

    spacePoints->push_back(sp);

}

//------------------------------------------------------------------------------

void TrackCandidate::sortSpacePointsByZ() {

    std::sort(spacePoints->begin(), spacePoints->end(), [](SpacePoint & a, SpacePoint & b) -> bool {
        return a.globalPos.Z() < b.globalPos.Z();
    });

}

//------------------------------------------------------------------------------

bool TrackCandidate::isValid() const {

    //TODO: read number of planes from input geometry
    //bool hitPlanes [N_PLANES] = {false};

    //for (const SpacePoint & sp : *spacePoints) {
    //    hitPlanes[sp.plane] = true;
    //}

    //for (bool hitPresent : hitPlanes) {
    //    if (!hitPresent) return false;
    //}

    return true;

}

//------------------------------------------------------------------------------

void TrackCandidate::dumpSpacePoints() const {

    cout << "INFO  FaserTracker::TrackCandidate::dumpSpacePoints\n"
         << "      Dumping space points in track candidate:\n";

    for (const SpacePoint & sp : *spacePoints) {
        //sp.print();
        const TVector3 & pos = sp.globalPos;
        cout << "        SpacePoint(x=" << pos.X()
             <<                  ", y=" << pos.Y()
             <<                  ", z=" << pos.Z()
             <<                  ")\n";
    }

}

//------------------------------------------------------------------------------

}

