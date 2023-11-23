# Compile the CMake files:
echo  -e "Compiling project...\n"

cmake -G "Ninja" -B "./cmake-build-debug/" -DCMAKE_BUILD_TYPE=DEBUG
ninja -C "./cmake-build-debug/"

# Variables
set videoInput "./resources/video/in/paint-demo.mp4"

# Sequential
set sequentialDemo "./cmake-build-debug/sequential_demo.out"
set sequentialVideoOutput "./resources/video/out/seq-compressed-paint-demo.mp4"

# OpenMP
set ompDemo "./cmake-build-debug/omp_demo.out"
set ompVideoOutput "./resources/video/out/omp-compressed-paint-demo.mp4"


# Execute the commands:

# Sequential:
echo -e "\e[33mStarting sequential test...\e[0m"
time $sequentialDemo $videoInput $sequentialVideoOutput
echo -e "\n"

# OpenMP:
echo -e "\e[33mStarting OpenMP tests...\n\e[0m"

echo -e "\e[33mTest with 2 threads: \e[0m"
time $ompDemo $videoInput $ompVideoOutput 2
echo -e "\n"

echo -e "\e[33mTest with 4 threads: \e[0m"
time $ompDemo $videoInput $ompVideoOutput 4
echo -e "\n"

echo -e "\e[33mTest with 8 threads: \e[0m"
time $ompDemo $videoInput $ompVideoOutput 8
echo -e "\n"

echo -e "\e[33mTest with 12 threads: \e[0m"
time $ompDemo $videoInput $ompVideoOutput 12
echo -e "\n"



