#!/bin/bash

# Instructions:
#   Set the following environment variables to the locations decribed below.

# Directory containing faser_tracker package
export FASER_BASE_DIR="$( cd "$(dirname "${BASH_SOURCE[0]}")" && pwd )"

# Directory containing cmake executable (comment out if not needed)
export PATH="$HOME/software/cmake-3.11.0-install/bin:$PATH"


echo "Configuring Geant4..."

# Geant4 package directory - obtain from http://www.geant4.org/geant4
export GEANT4_DIR="$HOME/software/geant4.10.04.p01"

# Geant4 build directory
export GEANT4_BUILD_DIR="$HOME/software/geant4.10.04.p01-build"

# Geant4 install directory
export GEANT4_INSTALL_DIR="$HOME/software/geant4.10.04.p01-install"

# Add Geant4 lib path to `LD_LIBRARY_PATH`
export LD_LIBRARY_PATH="$GEANT4_INSTALL_DIR/lib64:$LD_LIBRARY_PATH"

# Set up Geant4 datasets
echo "Setting up Geant4 datasets..."
source $GEANT4_INSTALL_DIR/share/Geant4-10.4.1/geant4make/geant4make.sh


#TODO: See if the following ACTS setup scripts can be used.
#      NB: They set up an incompatible version of ROOT even
#          if the ROOT setup below is used.
# ACTS
#echo "Configuring ACTS..."
#source $HOME/software/acts-core/CI/setup_lcg91.sh
#source $HOME/software/acts-core-install/bin/setup.sh


#TODO: Get faserMC package access working.
#      This requires adding cmake config to the faserMC package.
# faserMC package
#echo "Configuring faserMC..."
#export LD_LIBRARY_PATH="$HOME/Desktop/faser/run/lib:$LD_LIBRARY_PATH"


# ROOT configuration - see https://root.cern.ch
echo "Configuring ROOT..."
export ROOTSYS="$HOME/software/root"
export LD_LIBRARY_PATH="$ROOTSYS/lib:$LD_LIBRARY_PATH"
export PATH="$ROOTSYS/bin:$PATH"

echo "Done."

