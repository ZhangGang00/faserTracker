
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


add_executable(helix_tracker ./helix_tracker)
target_include_directories(helix_tracker PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(helix_tracker PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(helix_tracker PUBLIC ${PROJECT_SOURCE_DIR}/external/faserMC/faserMC/include)
target_link_libraries(helix_tracker PUBLIC FaserTrackerClassesDict)
target_link_libraries(helix_tracker PUBLIC FaserTracker)
target_link_libraries(helix_tracker PRIVATE ${Boost_LIBRARIES})

install(TARGETS helix_tracker RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


add_executable(plot_space_points ./plot_space_points)
target_include_directories(plot_space_points PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(plot_space_points PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(plot_space_points PUBLIC ${PROJECT_SOURCE_DIR}/external/faserMC/faserMC/include)
target_link_libraries(plot_space_points PUBLIC FaserTrackerClassesDict)
target_link_libraries(plot_space_points PUBLIC FaserTracker)
target_link_libraries(plot_space_points PRIVATE ${Boost_LIBRARIES})

install(TARGETS plot_space_points RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(helix_validation ./helix_validation)
target_include_directories(helix_validation PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(helix_validation PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(helix_validation PUBLIC ${PROJECT_SOURCE_DIR}/external/faserMC/faserMC/include)
target_link_libraries(helix_validation PUBLIC FaserTrackerClassesDict)
target_link_libraries(helix_validation PUBLIC FaserTracker)
target_link_libraries(helix_validation PRIVATE ${Boost_LIBRARIES})

install(TARGETS helix_validation RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(helix_2particle ./helix_2particle)
target_include_directories(helix_2particle PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(helix_2particle PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(helix_2particle PUBLIC ${PROJECT_SOURCE_DIR}/external/faserMC/faserMC/include)
target_link_libraries(helix_2particle PUBLIC FaserTrackerClassesDict)
target_link_libraries(helix_2particle PUBLIC FaserTracker)
target_link_libraries(helix_2particle PRIVATE ${Boost_LIBRARIES})

install(TARGETS helix_2particle RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(sp_separation ./sp_separation)
target_include_directories(sp_separation PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(sp_separation PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(sp_separation PUBLIC ${PROJECT_SOURCE_DIR}/external/faserMC/faserMC/include)
target_link_libraries(sp_separation PUBLIC FaserTrackerClassesDict)
target_link_libraries(sp_separation PUBLIC FaserTracker)
target_link_libraries(sp_separation PRIVATE ${Boost_LIBRARIES})

install(TARGETS sp_separation RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(plot_truth ./plot_truth)
target_include_directories(plot_truth PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(plot_truth PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(plot_truth PUBLIC ${PROJECT_SOURCE_DIR}/external/faserMC/faserMC/include)
target_link_libraries(plot_truth PUBLIC FaserTrackerClassesDict)
target_link_libraries(plot_truth PUBLIC FaserTracker)
target_link_libraries(plot_truth PRIVATE ${Boost_LIBRARIES})

install(TARGETS plot_truth RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(helix_tracker_TP ./helix_tracker_TP)
target_include_directories(helix_tracker_TP PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(helix_tracker_TP PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(helix_tracker_TP PUBLIC ${PROJECT_SOURCE_DIR}/external/faserMC/faserMC/include)
target_link_libraries(helix_tracker_TP PUBLIC FaserTrackerClassesDict)
target_link_libraries(helix_tracker_TP PUBLIC FaserTracker)
target_link_libraries(helix_tracker_TP PRIVATE ${Boost_LIBRARIES})

install(TARGETS helix_tracker_TP RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


