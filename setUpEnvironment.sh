#!/bin/bash

# Directory containing faserTracker package
export FASERTRACKER_DIR="$( cd "$(dirname "${BASH_SOURCE[0]}")" && pwd )"
export LD_LIBRARY_PATH="${FASERTRACKER_DIR}/faserTracker_run/lib64:$LD_LIBRARY_PATH"

## TODO: allow for more general options instead of hardcoding lxplus setup
## lxplus setup
platform=x86_64-slc6-gcc62-opt
view=/cvmfs/sft.cern.ch/lcg/views/LCG_90a/${platform}
#
source ${view}/setup.sh
#export BOOST_ROOT="${view}"
#export DD4hep_DIR="${view}"
#export EIGEN_INCLUDE_DIR="${view}/include/eigen3"
#export PYTHIA8_INCLUDE_DIR="${view}/include"
#export PYTHIA8_LIBRARY_DIR="${view}/lib"


################################################################################
# Location-specific settings: set the following environment variables to the as
# indicated.
#

# NB: The version of the default `cmake` with the above lxplus setup is too low.
# Directory containing cmake executable
#export PATH="$HOME/software/cmake-3.11.0-install/bin:$PATH"


# ROOT
echo "Configuring ROOT..."
#export ROOTSYS="$HOME/software/root"
#export LD_LIBRARY_PATH="$ROOTSYS/lib:$LD_LIBRARY_PATH:$ROOTSYS"
#export PATH="$ROOTSYS/bin:$PATH"


# End of location-specific settings
################################################################################

export PATH="$FASERTRACKER_DIR/faserTracker_run/bin:$PATH"

echo "Done."
