file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

add_executable(DroneShow_App WIN32 ${SOURCES})

target_link_libraries(DroneShow_App PRIVATE DroneShow_Game ECSEngine)
target_link_directories(DroneShow_App PRIVATE ${CMAKE_SOURCE_DIR}/External/DxLib)

add_custom_command(TARGET DroneShow_App POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/ECSEngine/data" "$<TARGET_FILE_DIR:DroneShow_App>/data"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/DroneShow_Game/data" "$<TARGET_FILE_DIR:DroneShow_App>/data"
    COMMENT "Copying data directories to output folder..."
)
