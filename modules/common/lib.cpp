#include <iostream>
#include <functional>
#include <opencv4/opencv2/opencv.hpp>
#include <omp.h>

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
cv::VideoWriter invoke_writer(const std::string& video_output_path, const VideoProps& video_props)
{
    const auto new_size =
            cv::Size(video_props.width / 3, video_props.height / 3);

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

cv::Mat compression_algorithm(cv::Mat& frame, const int reduction_factor)
{
    int new_height = frame.rows / reduction_factor;
    int new_width = frame.cols / reduction_factor;

    cv::Mat compressed_frame(new_height, new_width, frame.type());

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

cv::Mat omp_compression_algorithm(cv::Mat& frame, const int reduction_factor, const int n_threads) {
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







