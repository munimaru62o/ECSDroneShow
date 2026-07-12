cmake_minimum_required(VERSION 3.15)
project(ECSDroneShow VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")    
    add_compile_options(/utf-8 /bigobj)
    add_compile_definitions(UNICODE _UNICODE)
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

add_subdirectory(ECSEngine)
add_subdirectory(DroneShow_Game)
add_subdirectory(DroneShow_App)

enable_testing()
add_subdirectory(DroneShow_Test)
