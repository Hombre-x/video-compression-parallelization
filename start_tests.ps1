# Compile the CMake files:
Write-Output "Compiling project..."
cmake -G "Ninja" -B "./cmake-build-debug/" -DCMAKE_BUILD_TYPE=Debug
ninja -C "./cmake-build-debug/"

# Create variables:
$VideoInput = "./resources/video/in/paint-demo.mp4"

# Sequential:
$SequentialDemo = "./cmake-build-debug/SEQUENTIAL_DEMO.exe"
$SequentialVideoOutput = "./resources/video/out/seq-compressed-paint-demo.mp4"

# OpenMP:
$OMPDemo = "./cmake-build-debug/OMP_DEMO.exe"
$OMPVideoOutput = "./resources/video/out/omp-compressed-paint-demo.mp4"

# Execute the commands:

# Sequential:
Write-Host "Starting sequential test..." -ForegroundColor Yellow
Measure-Command -Expression {Invoke-Expression "$SequentialDemo $VideoInput $SequentialVideoOutput"}
Write-Host

# OpenMP:
Write-Host "Starting omp test with 2 threads..." -ForegroundColor Yellow
Measure-Command -Expression {Invoke-Expression "$OMPDemo $VideoInput $OMPVideoOutput 2"}
Write-Host

Write-Host "Startin omp test with 4 threads..." -ForegroundColor Yellow
Measure-Command -Expression {Invoke-Expression "$OMPDemo $VideoInput $OMPVideoOutput 4"}
Write-Host

Write-Host "Startin omp test with 8 threads..." -ForegroundColor Yellow
Measure-Command -Expression {Invoke-Expression "$OMPDemo $VideoInput $OMPVideoOutput 8"}
Write-Host

Write-Host "Startin omp test with 12 threads..." -ForegroundColor Yellow
Measure-Command -Expression {Invoke-Expression "$OMPDemo $VideoInput $OMPVideoOutput 12"}
Write-Host




