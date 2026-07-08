file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/External/OpenSimplex2/OpenSimplex2S.cpp")

add_library(ECSEngine STATIC ${SOURCES})

target_include_directories(ECSEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/External/DxLib
    ${CMAKE_SOURCE_DIR}/External/OpenSimplex2
)