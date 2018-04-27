#
# Based on G4 Example B1 and faserMC package.
#
#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(FASER_TRACKER)

#Use one or both of the following if C++14 is needed:
#set (CMAKE_CXX_STANDARD 14)
#ADD_DEFINITIONS(
#    -std=c++14
#)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Find ACTS
#find_package (ACTS COMPONENTS Core)
# Not needed:
#include($ENV{HOME}/software/acts-core-install/include)

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
message(STATUS "Geant4_USE_FILE=${Geant4_USE_FILE}")
include(${Geant4_USE_FILE})

#---------------------------------------------------------------------------
# Find ROOT
#

#list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

find_package(ROOT REQUIRED)
# ROOT version 6 
if(ROOT_FOUND)
  STRING(REGEX MATCH "6.*" VERSION6MATCH ${ROOT_VERSION})
  if(NOT VERSION6MATCH)
    message(FATAL_ERROR "ROOT 6 is required")
  endif()
endif()

#----------------------------------------------------------------------------
# Set up ROOT include directories and compile definitions
#

# Check ROOT settings in case they need to be debugged:
message(STATUS "ROOT_INCLUDE_DIRS=${ROOT_INCLUDE_DIRS}")
message(STATUS "ROOT_LIBRARIES=${ROOT_LIBRARIES}")
message(STATUS "ROOT_DEFINITIONS=${ROOT_DEFINITIONS}")
message(STATUS "ROOT_CXX_FLAGS=${ROOT_CXX_FLAGS}")
message(STATUS "ROOT_CC_FLAGS=${ROOT_CC_FLAGS}")
message(STATUS "ROOT_USE_FILE=${ROOT_USE_FILE}")

include(${ROOT_USE_FILE})

#---------------------------------------------------------------------------
#TODO: Use `FaserSensorHit` from faserMC package directly using something
#      similar to the lines below.
#      (This requires adding a cmake config to the faserMC package.)
#set(CMAKE_PREFIX_PATH $ENV{FASER_BASE_DIR}/faserMC)
#set(faserMC_DIR $ENV{FASER_BASE_DIR}/faserMC)
#include($ENV{FASER_BASE_DIR}/faserMC)
#find_package(faserMC REQUIRED)

#---------------------------------------------------------------------------
# Shared libraries are required (why?)
#
if(NOT Geant4_shared_FOUND)
  message(FATAL_ERROR "Geant must be built with shared libraries")
endif()

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
#TODO: Add faserMC includes once faserMC package access is enabled.
include_directories(${PROJECT_SOURCE_DIR}/include
                    #$ENV{FASER_BASE_DIR}/include
                    #$ENV{HOME}/software/acts-core-install/include
                    #$ENV{HOME}/software/eigen-eigen-5a0156e40feb
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIR}
                    "$ENV{HOME}/software/boost_1_66_0"
                    )
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/**.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/**.hh)

#---------------------------------------------------------------------------
# Generate dictionaries, add ROOT incantations
#
#TODO: Remove the following once these dictionaries can be accessed from the
#      faserMC package.
REFLEX_GENERATE_DICTIONARY(FaserRootClasses include/FaserRootClasses.hh SELECTION xml/selection.xml)
add_library(FaserRootClassesDict SHARED ${sources} FaserRootClasses.cxx)
set(libsuffix .so)
set(ROOT_LIBRARY_PROPERTIES ${ROOT_LIBRARY_PROPERTIES} SUFFIX ${libsuffix})
set_target_properties(FaserRootClassesDict PROPERTIES ${ROOT_LIBRARY_PROPERTIES})
target_link_libraries(FaserRootClassesDict ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(find_tracks find_tracks.cc ${sources} ${headers})
target_link_libraries(find_tracks FaserRootClassesDict ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS find_tracks DESTINATION bin)

#TODO: Safe to remove the following lines?
#install(TARGETS FaserRootClassesDict DESTINATION lib)
#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/FaserRootClasses_rdict.pcm DESTINATION lib)
#install(FILES ${FASER_SCRIPTS} DESTINATION .)

