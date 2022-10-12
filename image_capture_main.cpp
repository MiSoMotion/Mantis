#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>

#include "focuser.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/dnn/dnn.hpp>
// #include "zbar.h"

using namespace std;
using namespace cv;
// using namespace zbar;

namespace {

// std::string GetGStreamerPipeline(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method=0) {
//     return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(capture_width) + ", height=(int)" +
//            std::to_string(capture_height) + ", format=(string)NV12, framerate=(fraction)" + std::to_string(framerate) +
//            "/1 ! nvvidconv flip-method=" + std::to_string(flip_method) + " ! video/x-raw, width=(int)" + std::to_string(display_width) + ", height=(int)" +
//            std::to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
// }

std::string GetGStreamerPipeline(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method=0) {
    return std::string("v4l2src device=/dev/video0 ! video/x-raw, format=UYVY, width=1920, height=1080 ! videoconvert ! video/x-raw, format=BGR ! appsink"); 
} 
std::string GetGStreamerPipeline1(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method=0) {
    return std::string("v4l2src device=/dev/video1 ! video/x-raw, format=UYVY, width=1920, height=1080 ! videoconvert ! video/x-raw, format=BGR ! appsink"); 
}
std::string GetGStreamerPipeline2(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method=0) {
    return std::string("v4l2src device=/dev/video2 ! video/x-raw, format=UYVY, width=1920, height=1080 ! videoconvert ! video/x-raw, format=BGR ! appsink"); 
}
std::string GetGStreamerPipeline3(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method=0) {
    return std::string("v4l2src device=/dev/video3 ! video/x-raw, format=UYVY, width=1920, height=1080 ! videoconvert ! video/x-raw, format=BGR ! appsink"); 
}
std::string GetGStreamerPipeline4(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method=0) {
    return std::string("v4l2src device=/dev/video4 ! video/x-raw, format=UYVY, width=1920, height=1080 ! videoconvert ! video/x-raw, format=BGR ! appsink"); 
}
std::string GetGStreamerPipeline5(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method=0) {
    return std::string("v4l2src device=/dev/video5 ! video/x-raw, format=UYVY, width=1920, height=1080 ! videoconvert ! video/x-raw, format=BGR ! appsink"); 
}
constexpr int kImageWidth = 1920;
constexpr int kImageHeight = 1080;
constexpr int kFramerate = 10;
}  // namespace

// Image capture
int main(int argc, char **argv) {
    int cam_idx = 0;

    if (argc == 2) {
        cam_idx = atoi(argv[1]);
    }
    VideoCapture capture(GetGStreamerPipeline(/*capture_width=*/4032, /*capture_height=*/3040,
    /*display_width=*/4032, /*display_height=*/3040, /*frame_rate=*/10), cv::CAP_GSTREAMER);
    if (!capture.isOpened()) {
        cerr << "Could not open camera." << endl;
        exit(EXIT_FAILURE);
    }
    VideoCapture capture1(GetGStreamerPipeline1(/*capture_width=*/kImageWidth, /*capture_height=*/kImageHeight,
    /*display_width=*/kImageWidth, /*display_height=*/kImageHeight, /*frame_rate=*/kFramerate), cv::CAP_GSTREAMER);
    if (!capture1.isOpened()) {
        cerr << "Could not open camera." << endl;
        exit(EXIT_FAILURE);
    }
     VideoCapture capture2(GetGStreamerPipeline2(/*capture_width=*/kImageWidth, /*capture_height=*/kImageHeight,
    /*display_width=*/kImageWidth, /*display_height=*/kImageHeight, /*frame_rate=*/kFramerate), cv::CAP_GSTREAMER);
    if (!capture2.isOpened()) {
        cerr << "Could not open camera." << endl;
        exit(EXIT_FAILURE);
    }
    VideoCapture capture3(GetGStreamerPipeline3(/*capture_width=*/kImageWidth, /*capture_height=*/kImageHeight,
    /*display_width=*/kImageWidth, /*display_height=*/kImageHeight, /*frame_rate=*/kFramerate), cv::CAP_GSTREAMER);
    if (!capture3.isOpened()) {
        cerr << "Could not open camera." << endl;
        exit(EXIT_FAILURE);
    }
    VideoCapture capture4(GetGStreamerPipeline4(/*capture_width=*/kImageWidth, /*capture_height=*/kImageHeight,
    /*display_width=*/kImageWidth, /*display_height=*/kImageHeight, /*frame_rate=*/kFramerate), cv::CAP_GSTREAMER);
    if (!capture4.isOpened()) {
        cerr << "Could not open camera." << endl;
        exit(EXIT_FAILURE);
    }
    VideoCapture capture5(GetGStreamerPipeline5(/*capture_width=*/kImageWidth, /*capture_height=*/kImageHeight,
    /*display_width=*/kImageWidth, /*display_height=*/kImageHeight, /*frame_rate=*/kFramerate), cv::CAP_GSTREAMER);
    if (!capture5.isOpened()) {
        cerr << "Could not open camera." << endl;
        exit(EXIT_FAILURE);
    }
    namedWindow("Captured frame");
    namedWindow("Captured frame1");
    namedWindow("Captured frame2");
    namedWindow("Captured frame3");
    namedWindow("Captured frame4");
    namedWindow("Captured frame5");
    
    camera::Focuser focuser(/*bus_info=*/7, /*initial_focus_value=*/120);

    cv::Mat frame;
    capture >> frame;
    cv::Mat frame1;
    capture1 >> frame1;
    VideoWriter writer1;

    cv::Mat frame2;
    capture2 >> frame2;
    VideoWriter writer2;

    cv::Mat frame3;
    capture3 >> frame3;
    VideoWriter writer3;

    cv::Mat frame4;
    capture4 >> frame4;
    VideoWriter writer4;

    cv::Mat frame5;
    capture5 >> frame5;
    VideoWriter writer5;

    int skipped_frame_count = 0;
    // Capture an OpenCV frame
    while (true) {
        capture >> frame;
        capture1 >> frame1;
        capture2 >> frame2;
        capture3 >> frame3;
        capture4 >> frame4;
        capture5 >> frame5;
        auto t1 = std::chrono::steady_clock::now();
        auto t2 = std::chrono::steady_clock::now();
        if (frame.empty() || frame1.empty() || frame2.empty() || frame3.empty() || frame4.empty() || frame5.empty()) {
            break;
        }
        cv::Mat frame_resized;
        resize(frame, frame_resized, Size(640, 480));
        cv::Mat frame_resized1;
        resize(frame1, frame_resized1, Size(640, 480));
        cv::Mat frame_resized2;
        resize(frame2, frame_resized2, Size(640, 480));
        cv::Mat frame_resized3;
        resize(frame3, frame_resized3, Size(640, 480));
        cv::Mat frame_resized4;
        resize(frame4, frame_resized4, Size(640, 480));
        cv::Mat frame_resized5;
        resize(frame5, frame_resized5, Size(640, 480));
        
        imshow("Captured frame", frame_resized);
        imshow("Captured frame1", frame_resized1);
        imshow("Captured frame2", frame_resized2);
        imshow("Captured frame3", frame_resized3);
        imshow("Captured frame4", frame_resized4);
        imshow("Captured frame5", frame_resized5);


        auto focused = focuser.Focusing(frame);
        if (!focused) {
            continue;
        }
        int key = waitKey(1) & 0xff;
        // Stop the program on the ESC key.
        if (key == 27) {
            break;
        } else if (key == (int)('s')) {
            auto cppTimePoint = std::chrono::system_clock::now();
            auto cTimePoint = std::chrono::system_clock::to_time_t(cppTimePoint);
            string time_string(ctime(&cTimePoint));
            string image_path = "/home/motion/mantis_image_test/mantis_test_image" + time_string + ".png";
            string image_path1 = "/home/motion/mantis_image_test/mantis_test_image1" + time_string + ".png";
            string image_path2 = "/home/motion/mantis_image_test/mantis_test_image2" + time_string + ".png";
            string image_path3 = "/home/motion/mantis_image_test/mantis_test_image3" + time_string + ".png";
            string image_path4 = "/home/motion/mantis_image_test/mantis_test_image4" + time_string + ".png";
            string image_path5 = "/home/motion/mantis_image_test/mantis_test_image5" + time_string + ".png";

            std::cout << ctime(&cTimePoint) << std::endl;
            imwrite(image_path, frame);
            imwrite(image_path1, frame1);
            imwrite(image_path2, frame2);
            imwrite(image_path3, frame3);
            imwrite(image_path4, frame4);
            imwrite(image_path5, frame5);
        }
    }
    capture.release();
    capture1.release();
    capture2.release();
    capture3.release();
    capture4.release();
    capture5.release();
    return 0;
}