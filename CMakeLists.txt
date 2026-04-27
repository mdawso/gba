cmake_minimum_required(VERSION 4.3)
project(GBA)

set(TARGET_NAME GBA)

set(CXX_STANDARD 23)
set(CXX_STANDARD_REQUIRED YES)
set(CXX_EXTENSIONS OFF)

set(GBA_SOURCE_FILES 
    src/main.cpp
    src/types.hpp
    src/CPU_ARM7TDMI.cpp
    src/CPU_ARM7TDMI.hpp
    )
    
add_executable(${TARGET_NAME} ${GBA_SOURCE_FILES})

add_subdirectory(deps/raylib)
target_include_directories(${TARGET_NAME} PRIVATE deps/raylib/src)
target_link_libraries(${TARGET_NAME} PRIVATE raylib)