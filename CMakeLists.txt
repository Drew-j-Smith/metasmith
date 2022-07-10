cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

project(metasmith)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} src/main.cpp)
