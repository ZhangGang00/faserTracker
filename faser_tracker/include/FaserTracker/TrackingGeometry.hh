#pragma once

#include "FaserTracker/Settings.hh"
#include "TVector3.h"
#include <memory>
#include <vector>

class FaserTrackerGeometry;

namespace Acts {
    class RectangleBounds;
    class PlaneSurface;
    class Layer;
    class TrackingGeometry;
    class TrackingVolume;
}

//------------------------------------------------------------------------------

namespace FaserTracker {

    struct TrackingGeometry {

        std::shared_ptr<FaserTrackerGeometry> geo;

        std::shared_ptr<const Acts::RectangleBounds>        xyBounds;
        std::vector<std::shared_ptr<Acts::PlaneSurface>>    surfaces;
        std::vector<std::shared_ptr<Acts::Layer>>           layers;
        std::shared_ptr<Acts::TrackingVolume>               vacuumVolume;
        std::vector<std::shared_ptr<Acts::TrackingVolume>>  vacuumArray;
        std::shared_ptr<Acts::TrackingVolume>               world;

        int       nVacua = 20;
        double    eps = 10.; // mm
        TVector3  bField;

        TrackingGeometry(std::shared_ptr<Settings> settings);

        std::shared_ptr<Acts::TrackingGeometry>  actsGeo();

        TrackingGeometry(std::shared_ptr<FaserTrackerGeometry> geo);
        virtual ~TrackingGeometry();


    private:

        void  buildSurfaces();
        void  buildLayers();
        void  buildTrackingVolume();
        void  buildVacuumVolumes();
        void  glueVolumes();
        void  buildWorld();

        //Acts::MutableTrackingVolumePtr buildWorld(configParams& cfg, Acts::MutableTrackingVolumePtr& trackingVolume, std::array<Acts::MutableTrackingVolumePtr, nVacs>& vacuumVolumes);

    };

}

