#include <iostream>
#include <cstdlib>
#include <opencv2/imgproc.hpp>

#include "../common/lib.cpp"


int main(int argc, char* argv[])
{
    std::string input_path;
    std::string output_path;
    int n_threads;
    int frames_written = 0;

    if (argc > 3)
    {
        input_path  = argv[1];
        output_path = argv[2];
        n_threads = atoi(argv[3]);
    }
    else
    {
        std::cout << "[WARN] Not enough arguments passed, running with maximum threads and default paths." << '\n';
        input_path  = "../resources/video/in/paint-demo.mp4";
        output_path = "../resources/video/out/seq-compressed-paint-demo.mp4";
        n_threads = 12;
    }

    std::vector<cv::Mat> frames(n_threads);

    auto cap    = invoke_capture(input_path);

    int frames_written = 0;

    const auto video_props = VideoProps(cap);

    auto writer  = invoke_writer(output_path, video_props, 3);

    std::cout << "[INFO] Starting video compression..." << '\n';

    // Load frames

    std::cout << "Total frames: " << video_props.frame_count << '\n';

    do
    {
        for (int i = 0; i < frames.size(); i++)
        {
            cv::Mat frame;
            if(cap.read(frame))
            {
                frames[i] = frame;
                frames_written++;
            }

            else break;
        }

        std::vector<cv::Mat> compressed_frames = omp_compression_algorithm(frames, 3, n_threads);

        for (auto& frame : compressed_frames) writer.write(frame);

    }
    while (frames_written < video_props.frame_count);

    std::cout << "[INFO] Finishing video compression..." << '\n';

    cap.release();

    writer.release();

    return EXIT_SUCCESS;
}