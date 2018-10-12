//#pragma once
//
//#include <memory>
//#include "FaserTracker/Plots.hh"
//#include "FaserTracker/Settings.hh"
//#include "FaserTracker/DigiReader.hh"
//#include "TFile.h"
//
////class Settings;
////class DigiReader;
////class TFile;
//
//
//namespace FaserTracker {
//
//    struct Plots {
//
//        std::shared_ptr<Settings> settings;
//
//        Plots(std::shared_ptr<Settings> s) :
//            settings {s}
//        {
//        }
//    
//        void plotTruthDigits(const DigiReader & digiReader, TFile & outputFile, long iEvent);
//        // TODO: Eliminate `DigiReader` and just store `vector<Digit>` in faserMC output
//        //void plotDigits(const std::vector<Digit> & digits);
//
//        //void plotTruthDigits(const std::vector<Digit> & digits, int trackId, char xAxisVar, char yAxisVar);
//        //void compareDigiVsTruth(TGraph & gDigi, TGraph & gTruth);
//    
//    };
//
//}
//
