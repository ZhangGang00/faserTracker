#!/bin/bash

printUsage() {
    echo "Usage: source make.sh [--clean]"
}

makeFaserTracker() {

    if [ -z $FASER_TRACKER_DIR ]; then
        local faserTrackerDir="$( cd "$(dirname "${BASH_SOURCE[0]}")" && pwd )"
        echo 'Sourcing `'"${faserTrackerDir}/set_up_environment.sh"'` to set up the environment...'
        source ${faserTrackerDir}/set_up_environment.sh
    fi

    if [ "$2" != "" ]; then
        printUsage
        return 1
    fi
    if [ "$1" == "--clean" ]; then
        echo "Wiping ${FASER_TRACKER_DIR}/faser_tracker_build, ${FASER_TRACKER_DIR}/faser_tracker_run..."
        rm -rf ${FASER_TRACKER_DIR}/faser_tracker_build ${FASER_TRACKER_DIR}/faser_tracker_run
    elif [ "$1" != "" ]; then
        printUsage
        return 1
    fi

    cd $FASER_TRACKER_DIR
    echo "Now working in ${FASER_TRACKER_DIR}..."
    
    mkdir -p  faser_tracker_build  faser_tracker_run/include  faser_tracker_run/output

    # Avoid making `faserMC` a formal cmake dependency to avoid `Geant4` dependency
    #cp -f faser_tracker/external/faserMC/include/FaserTracker*.hh faser_tracker_run/include/

    cd faser_tracker_build
    echo "Now working in ${FASER_TRACKER_DIR}/faser_tracker_build..."
    
    #cmake -DGeant4_DIR="${GEANT4_INSTALL_DIR:?Need to set GEANT4_INSTALL_DIR}/lib64/Geant4-10.4.9" -DCMAKE_INSTALL_PREFIX="$FASER_TRACKER_DIR/faser_tracker_run" $FASER_TRACKER_DIR/faser_tracker -DCMAKE_C_COMPILER=/cvmfs/sft.cern.ch/lcg/contrib/gcc/7/x86_64-slc6/bin/gcc -DCMAKE_CXX_COMPILER=/cvmfs/sft.cern.ch/lcg/contrib/gcc/7/x86_64-slc6/bin/g++
    cmake -DCMAKE_INSTALL_PREFIX="${FASER_TRACKER_DIR}/faser_tracker_run" ${FASER_TRACKER_DIR}/faser_tracker
    make && make install

    #cp -i ${FASER_TRACKER_DIR}/faser_tracker/settings.json ${FASER_TRACKER_DIR}/faser_tracker_run/

    echo "Setup complete."
    
    cd ${FASER_TRACKER_DIR}/faser_tracker_run
    echo "Now working in ${FASER_TRACKER_DIR}/run..."
    echo "To process a ROOT file FILENAME containing faser TTree, run"
    echo "  ./bin/find_tracks -i FILENAME"
}

makeFaserTracker $@

cd ${FASER_TRACKER_DIR}
echo
echo "DEV --> Now back to ${FASER_TRACKER_DIR}"

