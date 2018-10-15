
add_executable(findTracks ./findTracks)
#target_include_directories(findTracks PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(findTracks PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(findTracks PUBLIC ${PROJECT_SOURCE_DIR}/external/faserMC/faserMC/include)
target_link_libraries(findTracks PUBLIC FaserTrackerClassesDict)
target_link_libraries(findTracks PUBLIC FaserTracker)
#target_link_libraries(findTracks PRIVATE ${Boost_LIBRARIES})
install(TARGETS findTracks RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(printTruth ./printTruth)
target_include_directories(printTruth PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(printTruth PUBLIC ${PROJECT_SOURCE_DIR}/external/faserMC/faserMC/include)
target_link_libraries(printTruth PUBLIC FaserTrackerClassesDict)
target_link_libraries(printTruth PUBLIC FaserTracker)
install(TARGETS printTruth RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(validateDigi ./validateDigi)
target_include_directories(validateDigi PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(validateDigi PUBLIC ${PROJECT_SOURCE_DIR}/external/faserMC/faserMC/include)
target_link_libraries(validateDigi PUBLIC FaserTrackerClassesDict)
target_link_libraries(validateDigi PUBLIC FaserTracker)
install(TARGETS validateDigi RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

