# faser_tracker

Keep your tracks from getting out of *fase*.


## Prerequisites

The following are required:
 * [cmake (version 2.6 or later)](https://cmake.org)
 * [ROOT (version 6)](https://root.cern.ch)
 * [Geant4](http://www.geant4.org/geant4)
 * [A Common Tracking Software (ACTS)](http://acts.web.cern.ch/ACTS)
 * [faserMC](https://github.com/asoffa/faserMC)


## Installation

After setting the environment variables in the `set_up_environment.sh` script
to the correct locations, run the following to set up the environment and compile
the `faser_tracker` package:

```
mv set_up_environment.sh make_faser_tracker.sh ..
cd ..
. set_up_environment.sh && . make_faser_tracker.sh
```

If the above commands complete successfully, you should be taken to the
`faser_tracker_run` directory, where you can run
```
./bin/find_tracks -i FILENAME [other flags]
```
where `FILENAME` is the name of a ROOT file produced by the `faserMC` package.

For a list of all available options, run
```
./bin/find_tracks --help
```

