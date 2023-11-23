#include <iostream>
#include <functional>
#include <opencv2/opencv.hpp>
#include <omp.h>


void println(auto any)
{
    std::cout << "[DEBUG] " << any << "\n";
}

/**
 * Applies the function f with potential side effects across
 * all the elements in the collections. It mutates the original
 * collection rather than return a new one
 *
 * @tparam T The type of the vector
 * @tparam U Potential return type of the function
 * @param vec Collection to apply the function f
 * @param f function of type T => U
 */
template<typename T, typename U>
void foreach(std::vector<T> &vec, std::function<U(T)> f)
{
    for (T& elem : vec)
    {
        f(elem);
    }
}

class VideoProps
{
    public:

        int width;
        int height;
        double fps;
        int frame_count;

        explicit VideoProps(cv::VideoCapture& cap)
        {
            const auto video_width_      = (int) cap.get(cv::CAP_PROP_FRAME_WIDTH);
            const auto video_height_     = (int) cap.get(cv::CAP_PROP_FRAME_HEIGHT);
            const auto video_fps_        = (int) cap.get(cv::CAP_PROP_FPS);
            const auto video_frame_count = (int) cap.get(cv::CAP_PROP_FRAME_COUNT);

            this -> width       = video_width_;
            this -> height      = video_height_;
            this -> fps         = video_fps_;
            this -> frame_count = video_frame_count;
        }
};

/**
 * Creates a capture object given an video path.
 * @param input_video_path Path from the root to the video
 * @return VideoCapture cap object
 */
cv::VideoCapture invoke_capture(const std::string& input_video_path)
{
    cv::VideoCapture cap(input_video_path); // Capture instance

    if(!cap.isOpened())
    {
        std::cout << "[ERROR] Error opening video stream or file" << '\n';
        exit(EXIT_FAILURE);
    }

    return cap;
}

/**
 * Creates a writer object with some given properties
 * @param video_output_path Output path for the future video
 * @param video_props Video properties
 * @return VideoWriter writer object
 */
cv::VideoWriter invoke_writer(const std::string& video_output_path, const VideoProps& video_props, auto reduction_factor)
{
    const auto new_size =
            cv::Size(video_props.width / reduction_factor, video_props.height / reduction_factor);

    cv::VideoWriter writer(
            video_output_path,
            cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
            video_props.fps,
            new_size
    );

    if (!writer.isOpened()) {
        std::cerr << "[ERROR] Could not the write object ... " << std::endl;
        exit(EXIT_FAILURE);
    }

    return writer;
}

cv::Mat compression_algorithm(cv::Mat& frame, const int reduction_factor) {
    int new_height = frame.rows / reduction_factor;
    int new_width = frame.cols / reduction_factor;

    cv::Mat compressed_frame(new_height, new_width, frame.type());

    for (int i = 0; i < new_height; i++) {
        for (int j = 0; j < new_width; j++) {
            int original_i = i * reduction_factor;
            int original_j = j * reduction_factor;

            compressed_frame.at<cv::Vec3b>(i, j) = frame.at<cv::Vec3b>(original_i, original_j);
        }
    }

    return compressed_frame;
}

cv::Mat omp_compression_algorithm_old(cv::Mat& frame, const int reduction_factor, const int n_threads) {
    int new_height = frame.rows / reduction_factor;
    int new_width = frame.cols / reduction_factor;

    cv::Mat compressed_frame(new_height, new_width, frame.type());


    #pragma omp parallel for num_threads(n_threads)
    for (int i = 0; i < new_height; ++i) {
        for (int j = 0; j < new_width; ++j) {
            cv::Vec3i sum(0, 0, 0);

            for (int x = i * reduction_factor; x < (i + 1) * reduction_factor; ++x) {
                for (int y = j * reduction_factor; y < (j + 1) * reduction_factor; ++y) {
                    // Use Vec3i for integer accumulation
                    sum += frame.at<cv::Vec3b>(x, y);
                }
            }

            // Calculate the average pixel value for the box
            cv::Vec3b average_pixel = cv::Vec3b(static_cast<uchar>(sum[0] / (reduction_factor * reduction_factor)),
                                                static_cast<uchar>(sum[1] / (reduction_factor * reduction_factor)),
                                                static_cast<uchar>(sum[2] / (reduction_factor * reduction_factor)));

            compressed_frame.at<cv::Vec3b>(i, j) = average_pixel;
        }
    }

    return compressed_frame;
}

/**
 * Method to load n_threads frames into video_frames
 * @param video_frames
 * @param cap
 * @param n_threads
 * @return
 */
std::vector<cv::Mat> load_frames(std::vector<cv::Mat>& video_frames, cv::VideoCapture& cap)
{
    cv::Mat frame;

    for (auto & video_frame : video_frames)
    {
        if(!cap.read(frame)) break;
        video_frame = frame;
    }

    return video_frames;
}

void write_frames(std::vector<cv::Mat>& video_frames, cv::VideoWriter& writer)
{
    for (const auto & video_frame : video_frames)
    {
        writer.write(video_frame);
    }
}

std::vector<cv::Mat> omp_compress_frames(std::vector<cv::Mat>& video_frames, int n_threads)
{
    std::vector<cv::Mat> compressed_frames(video_frames.size());

    #pragma omp parallel num_threads(n_threads)
    {
        int thread_id = omp_get_thread_num();
        cv::Mat compressed_frame;
        compressed_frame = compression_algorithm(video_frames[thread_id], 3);
        compressed_frames[thread_id] = compressed_frame;
    }

    return compressed_frames;
}



std::vector<cv::Mat> omp_compression_algorithm(std::vector<cv::Mat>& frames,
                                               const int reduction_factor,
                                               const int n_threads)
{
    std::vector<cv::Mat> compressed_frames(12);

    #pragma omp parallel num_threads(n_threads)
    {
        auto thread_id = omp_get_thread_num();

        compressed_frames[thread_id] = compression_algorithm(frames[thread_id], reduction_factor);

    }

    return compressed_frames;
}




