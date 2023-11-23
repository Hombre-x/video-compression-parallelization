#include <opencv4/opencv2/opencv.hpp>

typedef std::vector<uchar> UCMat;

/**
 * \brief Serializes a cv::Mat to a type that understands MPI
 * \param frame frame to be encoded
 * \param mat
 * \return
 */
UCMat serialize_frame(const cv::Mat& frame)
{
    UCMat buf;
    try
    {
        imencode(".png", frame, buf);
    }
    catch (cv::Exception& err)
    {
        std::cout << "[ERROR] Error in serialization function: " << err.what() << '\n';
    }

    return buf;
}

cv::Mat deserialize_frame(const UCMat& buf)
{
    try
    {
        cv::Mat compressed_frame = imdecode(buf, cv::IMREAD_COLOR);
        return compressed_frame;
    }
    catch (cv::Exception& err)
    {
        std::cout << "[ERROR] Error in deserialization function: " << err.what() << '\n';
        exit(EXIT_FAILURE);
    }
    return {};
}
