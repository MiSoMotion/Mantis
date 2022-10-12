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
constexpr int numCams = 6;

// std::string GetGStreamerPipeline(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method=0) {
//     return std::string("v4l2src device=/dev/video0 ! video/x-raw, format=UYVY, width=1920, height=1080 ! videoconvert ! video/x-raw, format=BGR ! appsink");
// }
// std::string GetGStreamerPipeline1(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method=0) {
//     return std::string("v4l2src device=/dev/video1 ! video/x-raw, format=UYVY, width=1920, height=1080 ! videoconvert ! video/x-raw, format=BGR ! appsink"); 
// }
// std::string GetGStreamerPipeline2(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method=0) {
//     return std::string("v4l2src device=/dev/video2 ! video/x-raw, format=UYVY, width=1920, height=1080 ! videoconvert ! video/x-raw, format=BGR ! appsink"); 
// }
// std::string GetGStreamerPipeline3(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method=0) {
//     return std::string("v4l2src device=/dev/video3 ! video/x-raw, format=UYVY, width=1920, height=1080 ! videoconvert ! video/x-raw, format=BGR ! appsink"); 
// }
// std::string GetGStreamerPipeline4(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method=0) {
//     return std::string("v4l2src device=/dev/video4 ! video/x-raw, format=UYVY, width=1920, height=1080 ! videoconvert ! video/x-raw, format=BGR ! appsink"); 
// }
// std::string GetGStreamerPipeline5(int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method=0) {
//     return std::string("v4l2src device=/dev/video5 ! video/x-raw, format=UYVY, width=1920, height=1080 ! videoconvert ! video/x-raw, format=BGR ! appsink"); 
// }

std::string gstreamerpipeline[numCams];
std::string gstream_enc[numCams];
std::thread threads[numCams];

VideoCapture capture;
VideoWriter writer;
cv::Mat frame;

}  // namespace

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
        
        capture.release();
        writer.release();
    }
}

// Image capture
int main(int argc, char **argv) {
    int cam_idx = 0;
    if (argc == 2) {
        cam_idx = atoi(argv[1]);
    }

   
    

    int num_cpus = std::thread::hardware_concurrency();
    std::cout << "Launching" << num_cpus << "threads\n";

    std::mutex iomutex;
    std::vector<std::thread> threads(num_cpus);



    for(int i = 0; i < numCams; i++){
        std::string camNum = std::to_string(i);
        gstreamerpipeline[i] =  std::string("v4l2src device=/dev/video" + camNum + " ! video/x-raw, format=UYVY, width=1920, height=1080 ! videoconvert ! video/x-raw, format=BGR ! appsink");
    }
   



    for (int i=0; i < numCams; i++){
        // threads[i] = std::thread([&iomutex,i]){
        //     std::lock_guard<std::mutex> iolock(iomutex);
        //     std::cout << "Thread #" << i << "is running\n";
        threads[i] = std::thread(video_capture, i);
        // }
        threads[i].join();
        
  }

//   for (auto& th: threads){
//     th.join();
//   }
   
       
   

    // VideoCapture capture(GetGStreamerPipeline(/*capture_width=*/kImageWidth, /*capture_height=*/kImageHeight,
    // /*display_width=*/kImageWidth, /*display_height=*/kImageHeight, /*frame_rate=*/kFramerate), cv::CAP_GSTREAMER);
    // if (!capture.isOpened()) {
    //     cerr << "Could not open camera." << endl;
    //     exit(EXIT_FAILURE);
    // }
    // VideoCapture capture1(GetGStreamerPipeline1(/*capture_width=*/kImageWidth, /*capture_height=*/kImageHeight,
    // /*display_width=*/kImageWidth, /*display_height=*/kImageHeight, /*frame_rate=*/kFramerate), cv::CAP_GSTREAMER);
    // if (!capture1.isOpened()) {
    //     cerr << "Could not open camera." << endl;
    //     exit(EXIT_FAILURE);
    // }
    //  VideoCapture capture2(GetGStreamerPipeline2(/*capture_width=*/kImageWidth, /*capture_height=*/kImageHeight,
    // /*display_width=*/kImageWidth, /*display_height=*/kImageHeight, /*frame_rate=*/kFramerate), cv::CAP_GSTREAMER);
    // if (!capture2.isOpened()) {
    //     cerr << "Could not open camera." << endl;
    //     exit(EXIT_FAILURE);
    // }
    // VideoCapture capture3(GetGStreamerPipeline3(/*capture_width=*/kImageWidth, /*capture_height=*/kImageHeight,
    // /*display_width=*/kImageWidth, /*display_height=*/kImageHeight, /*frame_rate=*/kFramerate), cv::CAP_GSTREAMER);
    // if (!capture3.isOpened()) {
    //     cerr << "Could not open camera." << endl;
    //     exit(EXIT_FAILURE);
    // }
    // VideoCapture capture4(GetGStreamerPipeline4(/*capture_width=*/kImageWidth, /*capture_height=*/kImageHeight,
    // /*display_width=*/kImageWidth, /*display_height=*/kImageHeight, /*frame_rate=*/kFramerate), cv::CAP_GSTREAMER);
    // if (!capture4.isOpened()) {
    //     cerr << "Could not open camera." << endl;
    //     exit(EXIT_FAILURE);
    // }
    // VideoCapture capture5(GetGStreamerPipeline5(/*capture_width=*/kImageWidth, /*capture_height=*/kImageHeight,
    // /*display_width=*/kImageWidth, /*display_height=*/kImageHeight, /*frame_rate=*/kFramerate), cv::CAP_GSTREAMER);
    // if (!capture5.isOpened()) {
    //     cerr << "Could not open camera." << endl;
    //     exit(EXIT_FAILURE);
    // }
    

    namedWindow("Captured frame");
    // namedWindow("Captured frame1");
    // namedWindow("Captured frame2");
    // namedWindow("Captured frame3");
    // namedWindow("Captured frame4");
    // namedWindow("Captured frame5");
    




    // cv::Mat frame;
    // capture >> frame;
    // VideoWriter writer;

    // string gstream_string = "appsrc ! video/x-raw, format=BGR ! queue ! videoconvert ! video/x-raw, format=BGRx ! nvvidconv ! video/x-raw(memory:NVMM), format=(string)I420, width=1920, height=1080 ! omxh264enc  qp-range=20,20:20,20:-1,-1 ! matroskamux ! queue ! filesink location=test2.mkv";
    
    // writer = VideoWriter(gstream_string, cv::CAP_GSTREAMER, 0, 15, {1920,1080});


    

    // cv::Mat frame1;
    // capture1 >> frame1;
    // VideoWriter writer1;

    // cv::Mat frame2;
    // capture2 >> frame2;
    // VideoWriter writer2;

    // cv::Mat frame3;
    // capture3 >> frame3;
    // VideoWriter writer3;

    // cv::Mat frame4;
    // capture4 >> frame4;
    // VideoWriter writer4;

    // cv::Mat frame5;
    // capture5 >> frame5;
    // VideoWriter writer5;

    // std::cout << "frame size " << frame.size()<< std::endl;
    // const int kCodec = VideoWriter::fourcc('M', 'P', '4', 'V');
    // string filename = "11.mp4";             // name of the output video file
    // writer.open("appsrc ! video/x-raw, format=(string)BGR, width=1920,height=1080 ! videoconvert ! video/x-raw, format=(string)I420, framerate=(fraction)15/1 ! omxh264enc preset-level=3 profile=8 insert-sps-pps=1 qp-range=8,24:8,24:8,24 ! video/x-h264,level=(string)5.2,stream-format=byte-stream ! h264parse ! qtmux ! filesink location=file.mp4" , filename, kCodec, static_cast<double>(kFramerate), frame.size(), true);
    // check if we succeeded
    // if (!writer.isOpened()) {
    //     cerr << "Could not open the output video file for write\n";
    //     return -1;
    // }
    
    // std::cout << "frame size " << frame.size() << std::endl;

    // // std::cout << "frame size " << frame1.size()<< std::endl;
    // std::cout << "frame size " << frame2.size()<< std::endl;
    // std::cout << "frame size " << frame3.size()<< std::endl;
    // std::cout << "frame size " << frame4.size()<< std::endl;
    // std::cout << "frame size " << frame5.size()<< std::endl;
    int skipped_frame_count = 0;

    // Capture an OpenCV frame
    while (true) {
        capture >> frame;
        // capture1 >> frame1;
        // capture2 >> frame2;
        // capture3 >> frame3;
        // capture4 >> frame4;
        // capture5 >> frame5;
        auto t1 = std::chrono::steady_clock::now();
        auto t2 = std::chrono::steady_clock::now();
        if (frame.empty()) {
            break;
        }

        // if (frame.empty() || frame1.empty() || frame2.empty() || frame3.empty() || frame4.empty() || frame5.empty()) {
        //     break;
        // }

        imshow("Captured frame", frame);
        // imshow("Captured frame1", frame1);
        // imshow("Captured frame2", frame2);
        // imshow("Captured frame3", frame3);
        // imshow("Captured frame4", frame4);
        // imshow("Captured frame5", frame5);


        // auto focused = focuser.Focusing(frame);
        // if (!focused) {
        //     continue;
        // }
        // writer.write(frame);
        // auto t3 = std::chrono::steady_clock::now();
        // cout << "capturing in ms:" << to_string(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()) << endl;
        // cout << "writing in ms:" << to_string(std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count()) << endl;
        
        
        
        //video encoding from nilcam to input into command line 
        // gst-launch-1.0 v4l2src device=/dev/video0 ! "video/x-raw, format=(string)UYVY, width=1920, height=1080" ! nvvidconv ! "video/x-raw(memory:NVMM), format=(string)I420, width=1920, height=1080" ! omxh264enc qp-range=20,20:20,20:-1,-1 ! matroskamux ! queue ! filesink location=video0.mkv v4l2src device=/dev/video1 ! "video/x-raw, format=(string)UYVY, width=1920, height=1080" ! nvvidconv ! "video/x-raw(memory:NVMM), format=(string)I420, width=1920, height=1080" ! omxh264enc qp-range=20,20:20,20:-1,-1 ! matroskamux ! queue ! filesink location=video1.mkv v4l2src device=/dev/video2 ! "video/x-raw, format=(string)UYVY, width=1920, height=1080" ! nvvidconv ! "video/x-raw(memory:NVMM), format=(string)I420, width=1920, height=1080" ! omxh264enc qp-range=20,20:20,20:-1,-1 ! matroskamux ! queue ! filesink location=video2.mkv v4l2src device=/dev/video3 ! "video/x-raw, format=(string)UYVY, width=1920, height=1080" ! nvvidconv ! "video/x-raw(memory:NVMM), format=(string)I420, width=1920, height=1080" ! omxh264enc qp-range=20,20:20,20:-1,-1 ! matroskamux ! q ueue ! filesink location=video3.mkv v4l2src device=/dev/video4 ! "video/x-raw, format=(string)UYVY, width=1920, height=1080" ! nvvidconv ! "video/x-raw(memory:NVMM), format=(string)I420, width=1920, height=1080" ! omxh264enc qp-range=20,20:20,20:-1,-1 ! matroskamux ! queue ! filesink location=video4.mkv v4l2src device=/dev/video5 ! "video/x-raw, format=(string)UYVY, width=1920, height=1080" ! nvvidconv ! "video/x-raw(memory:NVMM), format=(string)I420, width=1920, height=1080" ! omxh264enc qp-range=20,20:20,20:-1,-1 ! matroskamux ! queue ! filesink location=video5.mkv; 
        int key = waitKey(1) & 0xff;
        // Stop the program on the ESC key.
        if (key == 27) {
            break;
        }
    }
    



    // capture.release();
    // capture1.release();
    // capture2.release();
    // capture3.release();
    // capture4.release();
    // capture5.release();

    cout << "writing is done" << endl;
    // writer.release();
    // writer1.release();
    // writer2.release();
    // writer3.release();
    // writer4.release();
    // writer5.release();

    return 0;

}