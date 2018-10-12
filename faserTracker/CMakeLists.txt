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
#find_package(Boost 1.64 REQUIRED COMPONENTS program_options)
find_package(ROOT 6.10 REQUIRED COMPONENTS ${ROOT_COMPONENTS})

# faserMC
add_subdirectory(external/faserMC)


file(GLOB_RECURSE FaserTracker_files "src/*.cc" "include/*.hh")
add_library(FaserTracker SHARED ${FaserTracker_files})
target_include_directories (FaserTracker PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include>)
target_include_directories(FaserTracker PUBLIC ${ROOT_INCLUDE_DIRS})
target_include_directories(FaserTracker PUBLIC ${PROJECT_SOURCE_DIR}/external/faserMC/faserMC/include)
target_link_libraries(FaserTracker PUBLIC ${ROOT_LIBRARIES})
target_link_libraries(FaserTracker PUBLIC ${FaserTrackerClassesDict})
install(TARGETS FaserTracker LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

add_subdirectory(util)

