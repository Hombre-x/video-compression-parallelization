#include <iostream>
#include <cstdlib>
#include <opencv2/imgproc.hpp>

#include "../common/lib.cpp"

int main(int argc, char* argv[])
{
    std::string input_path;
    std::string output_path;

    if (argc > 2)
    {
        input_path  = argv[1];
        output_path = argv[2];
    }
    else
    {
        std::cout << "[WARN] Not enough arguments passed, using default video routes." << '\n';
        input_path  = "../resources/video/in/paint-demo.mp4";
        output_path = "../resources/video/out/seq-compressed-paint-demo.mp4";
    }


    auto cap    = invoke_capture(input_path);

    const auto video_props  = VideoProps(cap);

    auto writer  = invoke_writer(output_path, video_props, 3);

    std::cout << "[INFO] Starting video compression..." << '\n';


    for(int i = 0; i < video_props.frame_count; i++)
    {
        cv::Mat frame;
        cv::Mat compressed_frame;

        cap >> frame;
        compressed_frame = compression_algorithm(frame, 3);
        writer.write(compressed_frame);
    }

    std::cout << "[INFO] Finishing video compression..." << '\n';

    cap.release();
    writer.release();

    return EXIT_SUCCESS;
}
