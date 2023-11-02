cmake_minimum_required(VERSION 3.26)
project(Video_Compression)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CUDA_STANDARD 20)
enable_language(CUDA)


set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_TEST ${CMAKE_SOURCE_DIR} )

add_library(common STATIC ./modules/common/lib.cpp)
add_library(common_cuda STATIC ./modules/common/lib_cuda.cu)

target_link_libraries(common_cuda common)

find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

set(CV_LOG_STRIP_LEVEL "CV_LOG_LEVEL_ERROR")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    message(STATUS "Building for ${CMAKE_SYSTEM_NAME}")
    set(SEQUENTIAL_DEMO sequential_demo.out)
    set(OMP_DEMO omp_demo.out)

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    message(STATUS "Building for ${CMAKE_SYSTEM_NAME}")
    set(SEQUENTIAL_DEMO sequential_demo.exe)
    set(OMP_DEMO omp_demo.exe)
endif ()


set(CUDA_DEMO cuda_demo.exe)


# Sequential demo
add_executable(SEQUENTIAL_DEMO modules/sequential/video_reduction_sequential.cpp)
target_link_libraries(SEQUENTIAL_DEMO common)
target_link_libraries(SEQUENTIAL_DEMO ${OpenCV_LIBS})
target_link_libraries(SEQUENTIAL_DEMO OpenMP::OpenMP_CXX)

# OpenMP demo
add_executable(OMP_DEMO modules/openmp/video_reduction_omp.cpp)
target_link_libraries(OMP_DEMO common)
target_link_libraries(OMP_DEMO ${OpenCV_LIBS})
target_link_libraries(OMP_DEMO OpenMP::OpenMP_CXX)

# CUDA Demo
add_executable(CUDA_DEMO modules/cuda/video_reduction_cuda.cu)
set_target_properties(CUDA_DEMO PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
target_link_libraries(CUDA_DEMO common)
target_link_libraries(CUDA_DEMO common_cuda)
target_link_libraries(CUDA_DEMO ${OpenCV_LIBS})
