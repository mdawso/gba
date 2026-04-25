cmake_minimum_required(VERSION 3.15)
project(GBA VERSION 1.0.0 LANGUAGES C CXX)

add_subdirectory(deps/raylib)

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE deps/raylib/src)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)