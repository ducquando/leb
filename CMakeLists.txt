cmake_minimum_required(VERSION 3.14)
project(lebq_cpp CXX)
set(CMAKE_CXX_STANDARD 17)
set(SOURCES
    src/main_build.cpp
    src/main_query.cpp
    src/main_stats.cpp
)

include_directories(include)
include_directories(include/leb)

add_executable(main_build src/main_build.cpp)
add_executable(main_query src/main_query.cpp)
add_executable(main_stats src/main_stats.cpp)