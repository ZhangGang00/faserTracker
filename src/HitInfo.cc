#include "common_includes.hh"
#include "FaserSensorHit.hh"
#include "FaserTracker/HitInfo.hh"

namespace FaserTracker {

    void dumpHitInfo(const FaserSensorHit & hit) {

        G4ThreeVector globalPos = hit.GetGlobalPos();
        G4ThreeVector localPos  = hit.GetLocalPos();

        cout << "FaserSensorHit:\n"
             << "  trackID = " << hit.GetTrackID() << "\n"
             << "  planeID = " << hit.GetPlaneID() << "\n"
             << "  moduleID = " << hit.GetModuleID() << "\n"
             << "  sensorID = " << hit.GetSensorID() << "\n"
             << "  rowID    = " << hit.GetRowID() << "\n"
             << "  stripID  = " << hit.GetStripID() << "\n"
             << "  edep     = " << hit.GetEdep() << "\n"
             << "  globalPos = (" << globalPos.x() << ", "
                                  << globalPos.y() << ", "
                                  << globalPos.z() << ")\n"
             << "  localPos  = (" << localPos.x() << ", "
                                  << localPos.y() << ", "
                                  << localPos.z() << ")\n";
    }
}

