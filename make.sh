#!/bin/bash

#-------------------------------------------------------------------------------

printUsage() {
    echo "Usage: source make.sh [--clean]"
}

#-------------------------------------------------------------------------------

makeFaserTracker() {

    if [ -z $FASERTRACKER_DIR ]; then
        local faserTrackerDir="$( cd "$(dirname "${BASH_SOURCE[0]}")" && pwd )"
        echo 'Sourcing `'"${faserTrackerDir}/set_up_environment.sh"'` to set up the environment...'
        source ${faserTrackerDir}/set_up_environment.sh
    fi

    if [ "$2" != "" ]; then
        printUsage
        return 1
    fi
    if [ "$1" == "--clean" ]; then
        echo "Wiping ${FASERTRACKER_DIR}/faser_tracker_build, ${FASERTRACKER_DIR}/faser_tracker_run..."
        rm -rf ${FASERTRACKER_DIR}/faser_tracker_build ${FASERTRACKER_DIR}/faser_tracker_run
    elif [ "$1" != "" ]; then
        printUsage
        return 1
    fi

    cd $FASERTRACKER_DIR
    echo "Now working in ${FASERTRACKER_DIR}..."
    
    mkdir -p faser_tracker_build faser_tracker_run/include

    # Avoid making `faserMC` a formal cmake dependency to avoid `Geant4` dependency
    #cp -f faser_tracker/external/faserMC/include/FaserTracker*.hh faser_tracker_run/include/

    cd faser_tracker_build
    echo "Now working in ${FASERTRACKER_DIR}/faser_tracker_build..."
    
    cmake -DCMAKE_INSTALL_PREFIX="${FASERTRACKER_DIR}/faser_tracker_run" ${FASERTRACKER_DIR}/faser_tracker
    make && make install

    cp -i ${FASERTRACKER_DIR}/faser_tracker/settings.json ${FASERTRACKER_DIR}/faser_tracker_run/

    echo "Setup complete."
    
    cd ${FASERTRACKER_DIR}/faser_tracker_run
    echo "Now working in ${FASERTRACKER_DIR}/faser_tracker_run..."
    echo "To process a ROOT file FILENAME containing faser TTree, run"
    echo "  ./bin/find_tracks -i FILENAME"
}

#-------------------------------------------------------------------------------

makeFaserTracker $@


