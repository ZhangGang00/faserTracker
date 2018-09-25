#pragma once

#include <memory>
#include <string>

class TFile;
class TTree;

//------------------------------------------------------------------------------

namespace FaserTracker {

    class Settings;


    class IO {

        TTree * geoTree;
        TTree * evtTree;

    public:

        std::shared_ptr<Settings>  settings;
        std::unique_ptr<TFile>     inputFile;
        std::unique_ptr<TFile>     outputFile;
        TTree *                    eventTree();
        TTree *                    geometryTree();

        std::string faserDir();
        void loadFaserTrackerClasses();

        IO();
        IO(const std::string & settingsFile);

        virtual ~IO();

    };

}

