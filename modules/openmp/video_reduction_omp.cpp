#include <iostream>
#include <cstdlib>
#include <opencv4/opencv2/imgproc.hpp>

#include "../common/lib.cpp"


int main(int argc, char* argv[])
{
    const std::string input_path  = "./resources/video/in/paint-demo.mp4";
    const std::string output_path = "./resources/video/out/omp-compressed-paint-demo.mp4";
    int n_threads;

    if (argc > 1)
    {
        n_threads = atoi(argv[1]);
    }
    else
    {
        std::cout << "[WARN] Not enough arguments passed, running with maximum threads" << '\n';
        n_threads = 12;
    }

    auto cap    = invoke_capture(input_path);

    const auto video_props = VideoProps(cap);

    auto writer  = invoke_writer(output_path, video_props);

    std::cout << "[INFO] Starting video compression..." << '\n';

    for(int i = 0; i < video_props.frame_count; i++)
    {
        cv::Mat frame;
        cv::Mat compressed_frame;

        cap >> frame;

        compressed_frame = omp_compression_algorithm(frame, 3, n_threads);

        writer.write(compressed_frame);
    }

    std::cout << "[INFO] Finishing video compression..." << '\n';

    cap.release();
    writer.release();

    return EXIT_SUCCESS;
}