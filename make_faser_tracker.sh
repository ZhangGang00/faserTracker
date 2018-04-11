#!/bin/bash

cd ${FASER_BASE_DIR:?Need to set FASER_BASE_DIR}
echo "Now working in ${FASER_BASE_DIR}..."

mkdir -p faser_tracker_build faser_tracker_run
cd faser_tracker_build
echo "Now working in ${FASER_BASE_DIR}/faser_tracker_build..."

#cmake -DGeant4_DIR="${GEANT4_INSTALL_DIR:?Need to set GEANT4_INSTALL_DIR}/lib64/Geant4-10.4.9" -DCMAKE_INSTALL_PREFIX="$FASER_BASE_DIR/faser_tracker_run" $FASER_BASE_DIR/faser_tracker -DCMAKE_C_COMPILER=/cvmfs/sft.cern.ch/lcg/contrib/gcc/7/x86_64-slc6/bin/gcc -DCMAKE_CXX_COMPILER=/cvmfs/sft.cern.ch/lcg/contrib/gcc/7/x86_64-slc6/bin/g++
cmake -DGeant4_DIR="${GEANT4_INSTALL_DIR:?Need to set GEANT4_INSTALL_DIR}/lib64/Geant4-10.4.9" -DCMAKE_INSTALL_PREFIX="$FASER_BASE_DIR/faser_tracker_run" $FASER_BASE_DIR/faser_tracker
make && make install
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$FASER_BASE_DIR/run/lib"
echo "Setup complete."

cd ${FASER_BASE_DIR}/faser_tracker_run
echo "Now working in ${FASER_BASE_DIR}/run..."
echo "To process a ROOT file FILENAME containing faser TTree, run"
echo "  ./bin/find_tracks -i FILENAME"

