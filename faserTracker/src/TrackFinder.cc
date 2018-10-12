//#include "FaserTracker/common_includes.hh"
//#include "FaserTracker/TrackCandidate.hh"
//#include "FaserTracker/TrackFinder.hh"
//#include "FaserTracker/Digit.hh"
//#include "FaserTracker/DigiCluster.hh"
//#include "TCanvas.h"
//#include "TGraph.h"
//#include "TAxis.h"
//
//
//namespace FaserTracker {
//
//    vector<TrackCandidate> TrackFinder::findTruthTracks(const vector<Digit> & digits) {
//
//        // TODO: Lots of copying happening inside this function
//        //       --> Make more efficient if needed.
//
//
//        // Populate single-digit clusters
//
//        vector<DigiCluster> digitClusters;
//
//        for (const Digit & digit : digits) {
//
//            if (settings->tracks.truthIdStart > -1 &&
//                digit.truthTrackId < settings->tracks.truthIdStart)
//            {
//                continue;
//            }
//
//            if (settings->tracks.truthIdEnd > -1 &&
//                digit.truthTrackId > settings->tracks.truthIdEnd)
//            {
//                continue;
//            }
//
//            DigiCluster cluster {digit.plane};
//            cluster.addDigit(digit, true);
//            digitClusters.push_back(cluster);
//
//        }
//
//
//        // Add clusters with same truth track ID to `TrackCandidate`s
//
//        map<int, TrackCandidate> candidateMap;
//
//        for (const DigiCluster & cluster : digitClusters) {
//
//            int id = cluster.truthTrackId;
//
//            if (candidateMap.count(id) < 1) {
//                TrackCandidate candidate;
//                candidate.truthTrackId = id;
//                candidateMap[id] = candidate;
//            }
//
//            candidateMap[id].addCluster(cluster);
//
//        }
//
//
//        // Return track candidates
//
//        vector<TrackCandidate> candidates;
//
//        for (auto & it : candidateMap) {
//
//            if (!it.second.isValid()) continue;
//
//            it.second.label = "event"+to_string(it.second.eventNumber)
//                              + "_truthTrack"+to_string(it.first);
//
//            candidates.push_back(it.second);
//
//        }
//
//        return candidates;
//
//    }
//
//    ////////////////////////////////////////////////////////////////////////////////
//
//    vector<TrackCandidate> TrackFinder::findLinearYZ(const vector<Digit> & digits) {
//
//        vector<DigiCluster> digiClusters;
//
//        for (const Digit & digit : digits) {
//            DigiCluster cluster {digit.plane};
//            cluster.digits->push_back(digit);
//            digiClusters.push_back(cluster);
//        }
//
//        return findLinearYZ(digiClusters);
//
//    }
//
//    ////////////////////////////////////////////////////////////////////////////////
//
//    /**
//     * Find digit clusters across planes within a linear band of width `2*yTolerance`.
//     */
//    vector<TrackCandidate> TrackFinder::findLinearYZ(const vector<DigiCluster> & digiClusters) {
//
//        // First get collection of start/end digit clusters.
//
//        vector<DigiCluster> startClusters;
//        vector<DigiCluster> endClusters;
//        vector<DigiCluster> intermediateClusters;
//
//        for (const DigiCluster & cluster : digiClusters) {
//
//            if (cluster.charge < chargeThreshold) continue;
//
//            if (cluster.plane == START_PLANE) {
//                startClusters.push_back(cluster);
//            } else if (cluster.plane == END_PLANE) {
//                endClusters.push_back(cluster);
//            } else {
//                intermediateClusters.push_back(cluster);
//            }
//
//        }
//
//
//        // Now find intermediate clusters within `yTolerance` of line connecting
//        // start and end clusters to form track candidates.
//
//        // will return `trackCandidates`
//        vector<TrackCandidate> trackCandidates;
//
//        for (const DigiCluster & startCluster : startClusters) {
//            for (const DigiCluster & endCluster : endClusters) {
//
//                shared_ptr<TVector3> startPos = startCluster.globalPosition();
//                shared_ptr<TVector3> endPos = endCluster.globalPosition();
//
//                double slopeYZ = (endPos->Y() - startPos->Y()) /
//                                 (endPos->Z() - startPos->Z());
//
//                TrackCandidate candidate;
//                candidate.addCluster(startCluster);
//                candidate.addCluster(endCluster);
//
//                for (const DigiCluster & cluster : intermediateClusters) {
//                    shared_ptr<TVector3> pos = cluster.globalPosition();
//
//                    // Deviations in z negligible since large separation between
//                    // detector planes --> safe to neglect z uncertainties
//                    double yPredicted = startPos->Y() + slopeYZ*(pos->Z() - startPos->Z());
//                    if (fabs(pos->Y() - yPredicted) < yTolerance) {
//                        candidate.addCluster(cluster);
//                    }
//                }
//
//                if (candidate.isValid()) {
//
//                    candidate.label = "event"+to_string(candidate.eventNumber)
//                                      + "_clusterTrack"+to_string(_nTracksFound);
//
//                    trackCandidates.push_back(candidate);
//                    ++_nTracksFound;
//
//                }
//
//            }
//        }
//
//        return trackCandidates;
//
//    }
//
//    ////////////////////////////////////////////////////////////////////////////////
//
//    void TrackFinder::saveTrack(const TrackCandidate & trackCandidate) {
//
//        TGraph g_clusters;
//        for (const DigiCluster & cluster : *trackCandidate.digiClusters) {
//            shared_ptr<TVector3> pos = cluster.globalPosition();
//            g_clusters.SetPoint(g_clusters.GetN(), pos->Z(), pos->Y());
//        }
//
//        TCanvas canvas {"foundTrack", "foundTrack", 1000, 600};
//        canvas.SetGrid();
//        canvas.SetBatch(true);
//        g_clusters.Draw("ap*");
//
//        // Perform linear fit
//        g_clusters.Fit("pol1");
//
//        // Add labels
//        g_clusters.SetTitle(trackCandidate.label.c_str());
//        g_clusters.GetXaxis()->SetTitle("z [mm]");
//        g_clusters.GetYaxis()->SetTitle("y [mm]");
//
//        string saveName = "foundTrack_" + trackCandidate.label + ".png";
//        canvas.SaveAs(saveName.c_str());
//
//    }
//
//}
//
