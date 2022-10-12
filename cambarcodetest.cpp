#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <string>

#include "focuser.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "opencv2/dnn/dnn.hpp"



using namespace std;
using namespace cv;

namespace {

constexpr int kImageWidth = 1920;
constexpr int kImageHeight = 1080;
constexpr int kFramerate = 10;
constexpr int numCams = 2;

std::string gstreamerpipeline[numCams];
std::string gstream_enc[numCams];
std::thread threads[numCams];

VideoCapture capture;
VideoWriter writer;
cv::Mat frame;

void video_capture(int index) {
    VideoCapture capture(gstreamerpipeline[index], cv::CAP_GSTREAMER);
    while (true){
        capture >> frame;
         if (!capture.isOpened()) {
                    cerr << "Could not open camera." << endl;
                exit(EXIT_FAILURE);
            }
        
        std::string camIndex = std::to_string(index);
        gstream_enc[index] = std::string("appsrc ! video/x-raw, format=BGR ! queue ! videoconvert ! video/x-raw, format=BGRx ! nvvidconv ! video/x-raw(memory:NVMM), format=(string)I420, width=1920, height=1080 ! omxh264enc  qp-range=20,20:20,20:-1,-1 ! matroskamux ! queue ! filesink location=mantisvideo" + camIndex + ".mkv");
        writer = VideoWriter(gstream_enc[index], cv::CAP_GSTREAMER, 0, 15, {1920,1080});
        writer.write(frame);
        namedWindow("Captured frame");
        imshow("Captured frame", frame);

    }
}

int main(int argc, char **argv) {
    int cam_idx = 0;
    if (argc == 2) {
        cam_idx = atoi(argv[1]);
    }
   
    for(int i = 0; i < numCams; i++){
        std::string camNum = std::to_string(i);
        gstreamerpipeline[i] =  std::string("v4l2src device=/dev/video" + camNum + "! video/x-raw, format=UYVY, width=1920, height=1080 ! videoconvert ! video/x-raw, format=BGR ! appsink");
    }


    for (int i=0; i < numCams; i++){
        // threads[i] = std::thread([&iomutex,i]){
        //     std::lock_guard<std::mutex> iolock(iomutex);
        //     std::cout << "Thread #" << i << "is running\n";
        threads[i] = std::thread(video_capture, i);
        // }
        threads[i].join();
        
  }
    

    int skipped_frame_count = 0;

    // Capture an OpenCV frame
    while (true) {
        // capture >> frame;
        // auto t1 = std::chrono::steady_clock::now();
        // auto t2 = std::chrono::steady_clock::now();
        // if (frame.empty()) {
        //     break;
        // }
        // writer.write(frame);
        // imshow("Captured frame", frame);
 
        // auto focused = focuser.Focusing(frame);
        // if (!focused) {
        //     continue;
        // }
     
        // auto t3 = std::chrono::steady_clock::now();
        // cout << "capturing in ms:" << to_string(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()) << endl;
        // cout << "writing in ms:" << to_string(std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count()) << endl;
        
        
        
        //video encoding from nilcam to input into command line 
        // gst-launch-1.0 v4l2src device=/dev/video0 ! "video/x-raw, format=(string)UYVY, width=1920, height=1080" ! nvvidconv ! "video/x-raw(memory:NVMM), format=(string)I420, width=1920, height=1080" ! omxh264enc qp-range=20,20:20,20:-1,-1 ! matroskamux ! queue ! filesink location=video0.mkv v4l2src device=/dev/video1 ! "video/x-raw, format=(string)UYVY, width=1920, height=1080" ! nvvidconv ! "video/x-raw(memory:NVMM), format=(string)I420, width=1920, height=1080" ! omxh264enc qp-range=20,20:20,20:-1,-1 ! matroskamux ! queue ! filesink location=video1.mkv v4l2src device=/dev/video2 ! "video/x-raw, format=(string)UYVY, width=1920, height=1080" ! nvvidconv ! "video/x-raw(memory:NVMM), format=(string)I420, width=1920, height=1080" ! omxh264enc qp-range=20,20:20,20:-1,-1 ! matroskamux ! queue ! filesink location=video2.mkv v4l2src device=/dev/video3 ! "video/x-raw, format=(string)UYVY, width=1920, height=1080" ! nvvidconv ! "video/x-raw(memory:NVMM), format=(string)I420, width=1920, height=1080" ! omxh264enc qp-range=20,20:20,20:-1,-1 ! matroskamux ! q ueue ! filesink location=video3.mkv v4l2src device=/dev/video4 ! "video/x-raw, format=(string)UYVY, width=1920, height=1080" ! nvvidconv ! "video/x-raw(memory:NVMM), format=(string)I420, width=1920, height=1080" ! omxh264enc qp-range=20,20:20,20:-1,-1 ! matroskamux ! queue ! filesink location=video4.mkv v4l2src device=/dev/video5 ! "video/x-raw, format=(string)UYVY, width=1920, height=1080" ! nvvidconv ! "video/x-raw(memory:NVMM), format=(string)I420, width=1920, height=1080" ! omxh264enc qp-range=20,20:20,20:-1,-1 ! matroskamux ! queue ! filesink location=video5.mkv; 
        int key = waitKey(1) & 0xff;
        // Stop the program on the ESC key.
        if (key == 27) {
            break;
        // }  else if (key == (int)('s')) {
        //     auto cppTimePoint = std::chrono::system_clock::now();
        //     auto cTimePoint = std::chrono::system_clock::to_time_t(cppTimePoint);
        //     string time_string(ctime(&cTimePoint));
        //     string image_path = "/home/motion/mantis_image_test/barcodetest" + time_string + ".png";
           

        //     std::cout << ctime(&cTimePoint) << std::endl;
        //     imwrite(image_path, frame);

        // }
        
    }

    cout << "writing is done" << endl;
    capture.release();
    writer.release();
    return 0;
}