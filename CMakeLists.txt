cmake_minimum_required(VERSION 4.3)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS YES)
set(CMAKE_BUILD_TYPE Debug)

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
    src/Cartridge.cpp
    src/Cartridge.hpp
)
    
add_executable(${TARGET_NAME} ${GBA_SOURCE_FILES})

add_subdirectory(deps/raylib)
target_include_directories(${TARGET_NAME} PRIVATE deps/raylib/src)
target_link_libraries(${TARGET_NAME} PRIVATE raylib)