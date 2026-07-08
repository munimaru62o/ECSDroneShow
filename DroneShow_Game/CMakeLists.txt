file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.h")

add_library(DroneShow_Game STATIC ${SOURCES})

target_include_directories(DroneShow_Game PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/External/nlohmann
)

target_link_libraries(DroneShow_Game PUBLIC ECSEngine)
