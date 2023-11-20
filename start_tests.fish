# Compile the CMake files:
echo  -e "Compiling project...\n"

cmake -G "Ninja" -B "./cmake-build-debug/" -DCMAKE_BUILD_TYPE=DEBUG
ninja -C "./cmake-build-debug/"


# Execute the commands:

# Sequential:
echo -e "\e[33mStarting sequential test...\e[0m"
time ./sequential_demo.out
echo -e "\n"

# OpenMP:
echo -e "\e[33mStarting OpenMP tests...\n\e[0m"

echo -e "\e[33mTest with 2 threads: \e[0m"
time ./omp_demo.out 2
echo -e "\n"

echo -e "\e[33mTest with 4 threads: \e[0m"
time ./omp_demo.out 4
echo -e "\n"

echo -e "\e[33mTest with 8 threads: \e[0m"
time ./omp_demo.out 8
echo -e "\n"

echo -e "\e[33mTest with 12 threads: \e[0m"
time ./omp_demo.out 12
echo -e "\n"



