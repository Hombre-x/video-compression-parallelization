cmake_minimum_required(VERSION 3.26)
project(Video_Compression)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE TEST)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_TEST ${CMAKE_SOURCE_DIR} )

add_library(common STATIC ./modules/common/lib.cpp)

find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    message(STATUS "Building for ${CMAKE_SYSTEM_NAME}")
    set(SEQUENTIAL_DEMO sequential_demo.out)
    set(OMP_DEMO omp_demo.out)

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    message(STATUS "Building for ${CMAKE_SYSTEM_NAME}")
    set(SEQUENTIAL_DEMO sequential_demo.exe)
    set(OMP_DEMO omp_demo.exe)
endif ()


# Sequential demo
add_executable(SEQUENTIAL_DEMO modules/sequential/video_reduction_sequential.cpp)
target_link_libraries(SEQUENTIAL_DEMO common)
target_link_libraries(SEQUENTIAL_DEMO ${OpenCV_LIBS})

# OpenMP demo
add_executable(OMP_DEMO modules/openmp/video_reduction_omp.cpp)
target_link_libraries(OMP_DEMO common)
target_link_libraries(OMP_DEMO ${OpenCV_LIBS})
target_link_libraries(OMP_DEMO OpenMP::OpenMP_CXX)