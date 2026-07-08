cmake_minimum_required(VERSION 3.15)
project(ECSDroneShow VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")    
    add_compile_options(/utf-8 /bigobj)
    add_compile_definitions(UNICODE _UNICODE)
endif()

add_subdirectory(ECSEngine)
add_subdirectory(DroneShow_Game)
add_subdirectory(DroneShow_App)

enable_testing()
add_subdirectory(DroneShow_Test)
