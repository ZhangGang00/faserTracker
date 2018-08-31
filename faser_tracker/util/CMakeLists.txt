
add_executable(find_tracks ./find_tracks)
target_include_directories(find_tracks PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(find_tracks PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(find_tracks PUBLIC ${PROJECT_SOURCE_DIR}/external/faserMC/include)
#target_link_libraries(find_tracks PUBLIC FaserTrackerClassesDict)
target_link_libraries(find_tracks PUBLIC FaserTracker)
target_link_libraries(find_tracks PUBLIC ACTSCore)
target_link_libraries(find_tracks PRIVATE ${Boost_LIBRARIES})

install(TARGETS find_tracks RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

