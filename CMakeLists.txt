cmake_minimum_required(VERSION 3.16)
project(trading_sim CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB_RECURSE SRC_FILES src/*.cpp)

add_executable(simulator main.cpp ${SRC_FILES})
