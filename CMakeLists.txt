cmake_minimum_required(VERSION 3.26)
project(Video_Compression)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE TEST)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_TEST ${CMAKE_SOURCE_DIR} )

add_library(common STATIC ./modules/common/lib.cpp)

find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})


# Sequential demo
add_executable(sequential_demo.out modules/sequential/video_reduction_sequential.cpp)
target_link_libraries(sequential_demo.out common)
target_link_libraries(sequential_demo.out ${OpenCV_LIBS})

# OpenMP demo
add_executable(omp_demo.out modules/openmp/video_reduction_omp.cpp)
target_link_libraries(omp_demo.out common)
target_link_libraries(omp_demo.out ${OpenCV_LIBS})
target_link_libraries(omp_demo.out OpenMP::OpenMP_CXX)