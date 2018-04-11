//#include "common_includes.hh"
//#include "FaserTrackerBuildDetector.hh"
//
//#include "ACTS/Utilities/Logger.hpp"
//
//#include "ACTS/Detector/TrackingGeometry.hpp"
//#include "ACTS/Examples/GenericLayerBuilder.hpp"
//#include "ACTS/Material/Material.hpp"
//#include "ACTS/Tools/CylinderVolumeBuilder.hpp"
//#include "ACTS/Tools/CylinderVolumeHelper.hpp"
//#include "ACTS/Tools/LayerArrayCreator.hpp"
//#include "ACTS/Tools/LayerCreator.hpp"
//#include "ACTS/Tools/PassiveLayerBuilder.hpp"
//#include "ACTS/Tools/SurfaceArrayCreator.hpp"
//#include "ACTS/Tools/TrackingGeometryBuilder.hpp"
//#include "ACTS/Tools/TrackingVolumeArrayCreator.hpp"
//#include "ACTS/Utilities/Units.hpp"
//
//
//namespace FaserTracker {
//
//    unique_ptr<Acts::TrackingGeometry> buildDetector() {
//        Logging::Level surfaceLLevel = Logging::INFO;
//        Logging::Level layerLLevel   = Logging::INFO;
//        Logging::Level volumeLLevel  = Logging::INFO;
//        //size_t         stage;
//
//        //// configure surface array creator
//        //auto surfaceArrayCreator = make_shared<SurfaceArrayCreator>(getDefaultLogger("SurfaceArrayCreator", surfaceLLevel));
//        //// configure the layer creator that uses the surface array creator
//        //LayerCreator::Config lcConfig;
//        //lcConfig.surfaceArrayCreator = surfaceArrayCreator;
//        //auto layerCreator            = std::make_shared<LayerCreator>(
//        //        lcConfig, getDefaultLogger("LayerCreator", layerLLevel));
//        //// configure the layer array creator
//        //auto layerArrayCreator = std::make_shared<LayerArrayCreator>(
//        //        getDefaultLogger("LayerArrayCreator", layerLLevel));
//        //// tracking volume array creator
//        //auto tVolumeArrayCreator = std::make_shared<TrackingVolumeArrayCreator>(
//        //        getDefaultLogger("TrackingVolumeArrayCreator", volumeLLevel));
//        //// configure the cylinder volume helper
//        //CylinderVolumeHelper::Config cvhConfig;
//        //cvhConfig.layerArrayCreator          = layerArrayCreator;
//        //cvhConfig.trackingVolumeArrayCreator = tVolumeArrayCreator;
//        //auto cylinderVolumeHelper            = std::make_shared<CylinderVolumeHelper>(
//        //        cvhConfig, getDefaultLogger("CylinderVolumeHelper", volumeLLevel));
//        ////-------------------------------------------------------------------------------------
//        //// list the volume builders
//        //std::list<std::shared_ptr<ITrackingVolumeBuilder>> volumeBuilders;
//        //// a hash include for the Generic Detector : a bit ugly but effective
//        // "GenericDetector.ipp"
//        ////-------------------------------------------------------------------------------------
//        //// create the tracking geometry
//        //TrackingGeometryBuilder::Config tgConfig;
//        //tgConfig.trackingVolumeBuilders = volumeBuilders;
//        //tgConfig.trackingVolumeHelper   = cylinderVolumeHelper;
//        //auto cylinderGeometryBuilder
//        //    = std::make_shared<const TrackingGeometryBuilder>(
//        //            tgConfig, getDefaultLogger("TrackerGeometryBuilder", volumeLLevel));
//        //return cylinderGeometryBuilder->trackingGeometry();
//    }
//
//}
//
