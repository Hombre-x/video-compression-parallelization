#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/cuda.hpp>
#include <cuda_runtime.h>
#include <opencv2/core/cuda/common.hpp>

__global__ void cuda_compression_kernel(const uchar3* input, uchar3* output, int width, int height, int reduction_factor) {
    int new_height = height / reduction_factor;
    int new_width  = width  / reduction_factor;

    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < new_width && y < new_height) {
        int original_x = x * reduction_factor;
        int original_y = y * reduction_factor;

        output[y * new_width + x] = input[original_y * width + original_x];
    }
}

cv::Mat cuda_compression_algorithm(cv::Mat& frame, const int reduction_factor) {
    int width = frame.cols;
    int height = frame.rows;

    int new_height = height / reduction_factor;
    int new_width = width / reduction_factor;

    cv::Mat compressed_frame(new_height, new_width, frame.type());

    const auto in_bytes = frame.step * frame.rows;
    const auto out_bytes = compressed_frame.step * compressed_frame.rows;

    // Allocate device memory
    uchar3* d_input;
    uchar3* d_output;
    cudaMalloc((void**)&d_input, in_bytes);
    cudaMalloc((void**)&d_output, out_bytes);

    // Copy input data to device memory
    cudaMemcpy(d_input, frame.ptr<uchar3>(), width * height * sizeof(uchar3), cudaMemcpyHostToDevice);

    // Define block and grid dimensions
    const dim3 block(16,16);

    // Calculate grid size to cover the whole image
    const dim3 grid(cv::cuda::device::divUp(frame.cols, block.x), cv::cuda::device::divUp(frame.rows, block.y));

    // Launch the CUDA kernel
    cuda_compression_kernel<<<grid, block>>>(d_input, d_output, width, height, reduction_factor);

    // Synchronize to check for any kernel launch errors
    cudaSafeCall( cudaDeviceSynchronize() );

    // Copy output data from device memory
    cudaSafeCall(
            cudaMemcpy(
                    compressed_frame.ptr<uchar3>(),
                    d_output, new_width * new_height * sizeof(uchar3),
                    cudaMemcpyDeviceToHost)
    );

    // Free device memory
    cudaFree(d_input);
    cudaFree(d_output);

    return compressed_frame;
}


