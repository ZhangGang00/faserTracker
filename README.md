# faserTracker

Keep your tracks from getting out of *fase*.


## Prerequisites

The following are required:
 * [cmake (version 3.5 or later)](https://cmake.org)
 * [Boost (version 1.62 or later](https://www.boost.org)
 * [Eigen (version 3.2.9 or later)](http://eigen.tuxfamily.org)
 * [ROOT (version 6)](https://root.cern.ch)
 * [A Common Tracking Software (ACTS)](http://acts.web.cern.ch/ACTS)
 * [faserMC](https://github.com/asoffa/faserMC)

The `Boost`, `Eigen`, and `ROOT` dependencies are available by setting up the
default `lxplus` environment in `set_up_environment.sh` (see below), and the
`ACTS` and `faserMC` dependencies are installed by running the `make.sh` build
script (see below). Therefore, if working on lxplus, the only dependency that
needs manual installation is `cmake`.


## Installation

After setting the environment variables in the `set_up_environment.sh` script
to the correct locations, run
```
source make.sh  # sources `set_up_environment.sh`
```
to set up the environment and compile the `faserTracker` package.

If the above completes successfully, `faserTracker` should be installed and you
should be taken to the `faserTracker_run` directory, where you can run
```
./bin/find_tracks -i FILENAME [other flags]  # or just `find_tracks [flags]`
```
where `FILENAME` is the name of a ROOT file produced by the `faserMC` package.

To set up the environment in a new shell, run
```
source set_up_environment.sh
```

For a list of all available options, run
```
./bin/find_tracks --help
```

To do a clean rebuild of `faserTracker` run
```
source make.sh --clean
```
This wipes the `faserTracker_build` and `faserTracker_run` directories and
compiles `faserTracker` and its dependencies from scratch.


## Configuration settings

Configuration options are specified in the `settings.json` file **in the `faserTracker_run` directory**.

 | Feature       | Setting             | Description
 |:--------------|:--------------------|:-------------------------------------------
 |         input | fileName            | Name of file produced by `faserMC` tracking output.
 |               |                     | (default: `FaserMCEvent_tracking.root`)
 |               |                     |
 |               | eventTreeName       | Name of events tree from `faserMC` tracking output.
 |               |                     | (default: `events`)
 |               |                     |
 |               | geometryTreeName    | Name of geomtry tree from `faserMC` tracking output.
 |               |                     | (default: `geo`)
 |               |                     |
 |        output | fileName            | Set to `true` to print diagnostic info about the input chain.
 |               |                     |
 |         debug | chain               | Set to `true` to print diagnostic info about the input chain.
 |               |                     |
 |               | spacePoints         | Set to `true` to print diagnostic info about the space points.
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
 |  trackFinding | method              | Options:
 |               |                     |   `none`   | disable track finding
 |               |                     |   `truth`  | use truth tracks
 |               |                     |   `global` | use global chi-square approach
 |               |                     |   `seed`   | use seed finder
 |               |                     |   `all`    | use all of `truth`, `global`, and `seed` methods
 |               |                     |
 |  trackFitting | method              | Options (overridden to `none` if track finding is disabled)
 |               |                     |   `none`   | disable track fitting
 |               |                     |   `global` | use global chi-square approach
 |               |                     |   `Kalman` | use iterative Kalman filter approach
 |               |                     |   `all`    | use both `global` and `Kalman` methods

