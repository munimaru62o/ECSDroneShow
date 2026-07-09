include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
)
set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.cpp" "*.h")
add_executable(DroneShow_Test ${SOURCES})

target_include_directories(DroneShow_Test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(DroneShow_Test PRIVATE DroneShow_Game ECSEngine gtest_main)
target_link_directories(DroneShow_Test PRIVATE ${PROJECT_SOURCE_DIR}/External/DxLib)

add_test(NAME AllTests COMMAND DroneShow_Test)
