add_executable(plot_space_points ./plot_space_points)
target_include_directories(plot_space_points PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(plot_space_points PUBLIC ${PROJECT_SOURCE_DIR}/include)
#target_include_directories(plot_space_points PUBLIC ${PROJECT_SOURCE_DIR}/external/faserMC/include)
#target_link_libraries(plot_space_points PUBLIC FaserTrackerClassesDict)
target_link_libraries(plot_space_points PUBLIC FaserTracker)
target_link_libraries(plot_space_points PRIVATE ${Boost_LIBRARIES})
install(TARGETS plot_space_points RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


add_executable(find_tracks ./find_tracks)
target_include_directories(find_tracks PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(find_tracks PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(find_tracks PUBLIC FaserTracker)
target_link_libraries(find_tracks PRIVATE ${Boost_LIBRARIES})
install(TARGETS find_tracks RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


add_executable(fit_tracks ./fit_tracks)
target_include_directories(fit_tracks PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(fit_tracks PUBLIC ${PROJECT_SOURCE_DIR}/include)
#target_include_directories(fit_tracks PUBLIC ${PROJECT_SOURCE_DIR}/external/faserMC/include)
#target_link_libraries(fit_tracks PUBLIC FaserTrackerClassesDict)
target_link_libraries(fit_tracks PUBLIC FaserTracker)
target_link_libraries(fit_tracks PUBLIC ActsCore)
target_link_libraries(fit_tracks PRIVATE ${Boost_LIBRARIES})
install(TARGETS fit_tracks RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

