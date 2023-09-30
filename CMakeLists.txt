cmake_minimum_required(VERSION 3.26)
project(Video_Compression)

set(CMAKE_CXX_STANDARD 20)

# Sequential demo
add_executable(sequential_demo ./modules/sequential/playground.cc)