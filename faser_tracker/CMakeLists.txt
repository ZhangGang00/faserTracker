cmake_minimum_required(VERSION 3.5)

project(FaserTracker LANGUAGES CXX)

set(ROOT_COMPONENTS Core GenVector Hist Tree TreePlayer)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# required packages

# Enable C++11 threading support, with a preference for pthread
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(Boost 1.64 REQUIRED COMPONENTS program_options)
find_package(ROOT 6.10 REQUIRED COMPONENTS ${ROOT_COMPONENTS})

# faserMC
add_subdirectory(external/faserMC)

# acts-core and acts-fatras
set(ACTS_BUILD_DOC OFF CACHE BOOL "Build ACTS documentation")
set(ACTS_BUILD_EXAMPLES OFF CACHE BOOL "Build ACTS examples")
set(ACTS_BUILD_TESTS OFF CACHE BOOL "Build ACTS unit tests")
set(ACTS_BUILD_INTEGRATION_TESTS OFF CACHE BOOL "Build ACTS integration tests")
set(ACTS_BUILD_DD4HEP_PLUGIN OFF CACHE BOOL "Build ACTS DD4HEP plugins")
set(ACTS_BUILD_MATERIAL_PLUGIN ON CACHE BOOL "Build ACTS Material plugins")
set(ACTS_BUILD_TGEO_PLUGIN OFF CACHE BOOL "Build ACTS TGeo plugins")
add_subdirectory(external/acts-core)
#add_subdirectory(external/acts-fatras)

# simplify handling of optional components
# all arguments after the path are evaluated in the if(...) context
#function(add_subdirectory_if path)
#  file(RELATIVE_PATH _name ${PROJECT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/${path}")
#  if(${ARGN})
#    add_subdirectory(${path})
#    message(STATUS "Enable component '${_name}'")
#  else()
#    message(STATUS "Disable component '${_name}'")
#  endif()
#endfunction()
#
#add_subdirectory(Core)


#REFLEX_GENERATE_DICTIONARY(FaserTrackerClasses external/faserMC/include/FaserTrackerClasses.hh SELECTION external/faserMC/xml/tracker_selection.xml)
#add_library(FaserTrackerClassesDict SHARED ${sources} FaserTrackerClasses.cxx)
#set(libsuffix .so)
#set(ROOT_LIBRARY_PROPERTIES ${ROOT_LIBRARY_PROPERTIES} SUFFIX ${libsuffix})
#set_target_properties(FaserTrackerClassesDict PROPERTIES ${ROOT_LIBRARY_PROPERTIES})
#target_link_libraries(FaserTrackerClassesDict ${ROOT_LIBRARIES})


#file(GLOB_RECURSE FaserTrackerClasses_files "external/faserMC/src/FaserTracker*.cc" "external/faserMC/include/FaserTracker*.hh")
##message(STATUS "FaserTrackerClasses_files = ${FaserTrackerClasses_files}")
#add_library(FaserTrackerClassesDict SHARED ${FaserTrackerClasses_files})
#target_include_directories (FaserTrackerClassesDict PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/ ${CMAKE_CURRENT_SOURCE_DIR}/external/faserMC/include/> $<INSTALL_INTERFACE:include>)
#target_include_directories(FaserTrackerClassesDict PUBLIC ${ROOT_INCLUDE_DIRS})
#target_link_libraries(FaserTrackerClassesDict PUBLIC ${ROOT_LIBRARIES})
#install(TARGETS FaserTrackerClassesDict LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})


file(GLOB_RECURSE FaserTracker_files "src/*.cc" "include/*.hh")
add_library(FaserTracker SHARED ${FaserTracker_files})
target_include_directories (FaserTracker PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories(FaserTracker PUBLIC ${ROOT_INCLUDE_DIRS})
target_link_libraries(FaserTracker PUBLIC ActsCore)
target_link_libraries(FaserTracker PUBLIC ${ROOT_LIBRARIES})
install(TARGETS FaserTracker LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

#install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

add_subdirectory(util)

