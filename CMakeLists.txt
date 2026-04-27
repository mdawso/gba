cmake_minimum_required(VERSION 4.3)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

project(GBA)

set(TARGET_NAME GBA)

set(GBA_SOURCE_FILES 
    src/main.cpp
    src/Types.hpp
    src/CPU_ARM7TDMI.cpp
    src/CPU_ARM7TDMI.hpp
    src/Bus.cpp
    src/Bus.hpp
    src/Helpers.hpp
    )
    
add_executable(${TARGET_NAME} ${GBA_SOURCE_FILES})

add_subdirectory(deps/raylib)
target_include_directories(${TARGET_NAME} PRIVATE deps/raylib/src)
target_link_libraries(${TARGET_NAME} PRIVATE raylib)