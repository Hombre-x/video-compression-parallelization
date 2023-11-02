#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/cuda.hpp>

#include "../common/lib_cuda.cu"
#include "../common/lib.cpp"


int main()
{
    const std::string input_path  = "../resources/video/in/paint-demo.mp4";
    const std::string output_path = "../resources/video/out/cuda-compressed-paint-demo.mp4";

    auto cap    = invoke_capture(input_path);

    const auto video_props  = VideoProps(cap);

    auto writer  = invoke_writer(output_path, video_props);

    std::cout << "[INFO] Starting video compression..." << '\n';


    for(int i = 0; i < video_props.frame_count; i++)
    {
        cv::Mat frame;
        cv::Mat compressed_frame;

        if(!cap.read(frame)) break;
        compressed_frame = cuda_compression_algorithm(frame, 3);
        writer.write(compressed_frame);
    }

    std::cout << "[INFO] Finishing video compression..." << '\n';

    cap.release();
    writer.release();

    return EXIT_SUCCESS;
}
