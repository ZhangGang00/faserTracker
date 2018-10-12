#!/bin/bash

#-------------------------------------------------------------------------------

printUsage() {
    echo "Usage: source make.sh [--clean]"
}

#-------------------------------------------------------------------------------

makeFaserTracker() {

    if [ -z $FASERTRACKER_DIR ]; then
        local faserTrackerDir="$( cd "$(dirname "${BASH_SOURCE[0]}")" && pwd )"
        echo 'Sourcing `'"${faserTrackerDir}/setUpEnvironment.sh"'` to set up the environment...'
        source ${faserTrackerDir}/setUpEnvironment.sh
    fi

    if [ "$2" != "" ]; then
        printUsage
        return 1
    fi
    if [ "$1" == "--clean" ]; then
        echo "Wiping ${FASERTRACKER_DIR}/faserTracker_build, ${FASERTRACKER_DIR}/faserTracker_run..."
        rm -rf ${FASERTRACKER_DIR}/faserTracker_build ${FASERTRACKER_DIR}/faserTracker_run
    elif [ "$1" != "" ]; then
        printUsage
        return 1
    fi

    cd $FASERTRACKER_DIR
    echo "Now working in ${FASERTRACKER_DIR}..."
    
    mkdir -p faserTracker_build faserTracker_run/include

    # Avoid making `faserMC` a formal cmake dependency to avoid `Geant4` dependency
    #cp -f faserTracker/external/faserMC/include/FaserTracker*.hh faserTracker_run/include/

    cd faserTracker_build
    echo "Now working in ${FASERTRACKER_DIR}/faserTracker_build..."
    
    cmake -DCMAKE_INSTALL_PREFIX="${FASERTRACKER_DIR}/faserTracker_run" ${FASERTRACKER_DIR}/faserTracker
    make && make install

    cp -i ${FASERTRACKER_DIR}/faserTracker/settings.json ${FASERTRACKER_DIR}/faserTracker_run/

    echo "Setup complete."
    
    cd ${FASERTRACKER_DIR}/faserTracker_run
    echo "Now working in ${FASERTRACKER_DIR}/faserTracker_run..."
    echo "To process a ROOT file FILENAME containing faser TTree, run"
    echo "  ./bin/find_tracks -i FILENAME"
}

#-------------------------------------------------------------------------------

makeFaserTracker $@


