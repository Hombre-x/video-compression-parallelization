cmake_minimum_required(VERSION 3.26)
project(Video_Compression)

set(CMAKE_CXX_STANDARD 20)
add_library(common STATIC ./modules/common/lib.cpp
        modules/openmp/video_reduction_omp.cpp)

find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})


# Sequential demo
add_executable(sequential_demo modules/sequential/video_reduction_sequential.cpp)
target_link_libraries(sequential_demo common)
target_link_libraries(sequential_demo ${OpenCV_LIBS})

# OpenMP demo
add_executable(omp_demo modules/openmp/video_reduction_omp.cpp)
target_link_libraries(omp_demo common)
target_link_libraries(omp_demo ${OpenCV_LIBS})
target_link_libraries(omp_demo OpenMP::OpenMP_CXX)