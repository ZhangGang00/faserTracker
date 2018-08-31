# faser_tracker

Keep your tracks from getting out of *fase*.


## Prerequisites

The following are required:
 * [cmake (version 3.5 or later)](https://cmake.org)
 * [ROOT (version 6)](https://root.cern.ch)
 * [Geant4](http://www.geant4.org/geant4)
 * [A Common Tracking Software (ACTS)](http://acts.web.cern.ch/ACTS)
 * [faserMC](https://github.com/asoffa/faserMC)


## Installation

After setting the environment variables in the `set_up_environment.sh` script
to the correct locations, run
```
source make.sh  # sources `set_up_environment.sh`
```
to set up the environment and compile the `faser_tracker` package.

If the above completes successfully, `faser_tracker` should be installed and you
should be taken to the `faser_tracker_run` directory, where you can run
```
./bin/find_tracks -i FILENAME [other flags]
```
where `FILENAME` is the name of a ROOT file produced by the `faserMC` package.

For a list of all available options, run
```
./bin/find_tracks --help
```

To do a clean rebuild of `faser_tracker` run
```
source make.sh --clean
```
This wipes the `faser_tracker_build` and `faser_tracker_run` directories and
compiles `faser_tracker` and its dependencies from scratch.


## Configuration settings

Configuration options are specified in the `settings.json` file **in the `faser_tracker_run` directory**.

 | Feature       | Setting             | Description
 |:--------------|:--------------------|:-------------------------------------------
 |        debug  | chain               | Set to `true` to print diagnostic info about the input chain.
 |               |                     |
 |               | hits                | Set to `true` to print diagnostic info about the hits.
 |               |                     |
 |               | digits              | Set to `true` to print diagnostic info about the digits.
 |               |                     |
 |               | tracks              | Set to `true` to print diagnostic info about the tracks.
 |               |                     |
 |        events | eventNumberStart    | Event (entry) number to start at (-1 to start with first event)
 |               |                     |
 |               | eventNumberEnd      | Event (entry) number to end at (-1 to end with last event)
 |               |                     |
 |        tracks | truthIdStart        | Starting truth ID of tracks to consider (-1 to start with lowest)
 |               |                     |
 |               | truthIdEnd          | Ending truth ID of tracks to consider (-1 to end with highest)
 |               |                     |
 |               | countTracks         | Set to `true` to print the total number of tracks
 |               |                     |
 |               | fitTracks           | Set to `true` to perform a global track fit in x vs. z for the hits or digits.
 |               |                     |
 |               | plotHits            | Set to `true` to plot hit positions.
 |               |                     |
 |               | plotDigits          | Set to `true` to plot digit position.
 |               |                     |
 |               | plotTruth           | Set to `true` to plot truth positions.
 |               |                     |
 |    clustering | distanceTolerance   | Maximum distance (in mm) allowed between points in cluster
 |               |                     |
 |               | dumpClusters        | Set to `true` to print information on clusters found.
 |               |                     |
 |               | dumpTruthTrackIds   | Set to `true` to print truth track IDs of digits in clusters.
 |               |                     |
 |  trackFinding | chargeThreshold     | Minimum charge (in fC) required for digit clusters
 |               |                     |
 |               | yTolerance          | Minimum distance (in mm) allowed for linear y-vs.-z band
 |               |                     |
 |               | findTruthTracks     | Set to `true` to find tracks using truth track IDs.
 |               |                     |
 |               | findClusterTracks   | Set to `true` to find tracks from digit clusters.
 |               |                     |
 |               | saveTracks          | Set to `true` to save y-vs.-z plots of found tracks.
 |               |                     |
 |  trackFitting | fitAndSaveTracks    | Set to `true` to fit and save x-vs.-z plots of tracks found.
  
