#include "FaserTracker/common_includes.hh"
#include "FaserTracker/Digit.hh"
//#include "FaserTracker/DigiReader.hh"
//#include "TVector3.h"
//#include "TMath.h"


namespace FaserTracker {

    shared_ptr<TVector3> Digit::globalPosition() const {
        if (_cachedGlobalPosition != nullptr) {
            return _cachedGlobalPosition;
        }

        _cachedGlobalPosition = make_shared<TVector3>(truthGlobalPos);
        return _cachedGlobalPosition;
    }


//    //TODO: Read this info in from faserMC/faserGeo.mac instead of hardcoding.
//    const int    N_READOUT_STRIPS = 1280;
//    const int    N_SENSOR_PLANES = 5;
//
//    const double STEREO_ANGLE   = 0.026;
//    const double PLANE_SPACING  = 0.2;
//    const double PLANE_WIDTH    = ;
//    const double SUPPORT_SIZE_Z = 0.0033;
//    const double SENSOR_SIZE_X  = ;
//    const double SENSOR_SIZE_Z  = 0.32;
//    const double STRIP_LENGTH   = 0.0482;
//    const double STRIP_PITCH    = 75.5e-6;
//    const double SENSOR_GAP     = 269e-6;
//
//    shared_ptr<TVector3> Digit::globalPosition() {
//        double sensorX = (N_READOUT_STRIPS + 2) * STRIP_PITCH + SENSOR_GAP;
//        double sensorY = 2.0 * (STRIP_LENGTH + SENSOR_GAP);
//        double sensorZ = SENSOR_SIZE_Z;
//
//        // First find the plane position in world coordinates.
//        // --> front of first plane at z=0 in world coordinates
//        TVector3 planePos = {0, 0, digit.plane * PLANE_SPACING};
//
//        // Now find the module position relative to the plane position.
//        double wPrime = 0.5 * SENSOR_SIZE_X / TMath::Cos(STEREO_ANGLE);
//        double overlapAngle = 0.5 * TMath::ASin(MODULE_SIZE_Z/wPrime);
//        double moduleX = wPrime * TMath::Cos(overlapAngle);
//        if (digit.module==0) moduleX = -moduleX;
//        TVector3 modulePos = {moduleX, 0, 0};
//
//        // Now find the sensor position relative to the module position.
//        double sensorY = 0.5 * sensorY / TMath::Cos(STEREO_ANGLE);
//        double sensorZ = 0.5 * (SUPPORT_SIZE_Z + SENSOR_SIZE_Z);
//        if (digit.sensor==2 || digit.sensor==3) sensorY = -sensorY;
//        if (digit.sensor==1 || digit.sensor==3) sensorZ = -sensorZ;
//        TVector3 sensorPos = {sensorX, sensorY, sensorZ};
//
//        // Now find the row position relative to the module position.
//        double rowY = 0.5 * (STRIP_LENGTH + SENSOR_GAP);
//        if (digit.row==0) rowY = -rowY;
//        TVector3 rowPos = {0, rowY, 0};
//
//        // Now find the strip position relative to the row position.
//        double stripX = (digit.strip - N_READOUT_STRIPS/2) * N_READOUT_STRIPS;
//        TVector3 stripPos = {stripX, 0, 0};
//
//        return planePos + modulePos + sensorPos + rowPos + stripPos;
//    }

}



