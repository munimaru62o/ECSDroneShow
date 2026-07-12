file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.h")
list(APPEND SOURCES "${PROJECT_SOURCE_DIR}/External/OpenSimplex2/OpenSimplex2S.cpp")

add_library(ECSEngine STATIC ${SOURCES})

target_include_directories(ECSEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/External/OpenSimplex2
)
