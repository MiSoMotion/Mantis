#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <string>
#include <condition_variable>
#include "focuser.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/dnn/dnn.hpp"
#include <mutex>

using namespace std;
using namespace cv;

namespace {

constexpr int kImageWidth = 1920;
constexpr int kImageHeight = 1080;
constexpr int kFramerate = 10;
constexpr int numCams = 6; 


std::string gstreamerpipeline[numCams];
std::string gstream_enc[numCams];
std::mutex mu;
std::condition_variable condition;
int g_counter = 0;

}  // namespace

//function to be rendered in thread capturing each camera frame, writing, and then displaying
void video_capture(int index) {
    std::unique_lock<mutex> lock(mu);
    g_counter++;
    mu.unlock();

    VideoCapture capture(gstreamerpipeline[index], cv::CAP_GSTREAMER);
    std::string camIndex = std::to_string(index);
    gstream_enc[index] = std::string("appsrc ! video/x-raw, format=BGR ! queue ! videoconvert ! video/x-raw, format=BGRx ! nvvidconv ! video/x-raw(memory:NVMM), format=(string)I420, width=1920, height=1080 ! omxh264enc  qp-range=20,20:20,20:-1,-1 ! matroskamux ! queue ! filesink location=mantisvideo" + camIndex + ".mkv");
    VideoWriter writer = VideoWriter(gstream_enc[index], cv::CAP_GSTREAMER, 0, 15, {1920,1080});
    cv::Mat frame;
    while (true){    
        capture >> frame;
        if (!capture.isOpened()) {
            cerr << "Could not open camera." << endl;
            exit(EXIT_FAILURE);
        } if (frame.empty()) {
            break;
        }
        if (g_counter < numCams) {
            continue;
        }
        imshow("Captured frame" + camIndex, frame);
        namedWindow("Captured frame" + camIndex);
        if (g_counter == numCams){
            writer.write(frame);
        }

        int key = waitKey(1) & 0xff;
        // Stop the program on the ESC key.
        if (key == 27) {
            break;
        }
    }
    capture.release();
    writer.release();
    // mu.unlock();
}

// Image capture
int main(int argc, char **argv) {
    int cam_idx = 0;
    if (argc == 2) {
        cam_idx = atoi(argv[1]);
    }

   
//initializing threads
    int num_cpus = std::thread::hardware_concurrency();
    std::cout << "Launching" << num_cpus << "threads\n";

    std::mutex iomutex;
    std::vector<std::thread> threads;


    // array of strings containing gstream pipeline for each camera
    for(int i = 0; i < numCams; i++){
        std::string camNum = std::to_string(i);
        gstreamerpipeline[i] =  std::string("v4l2src device=/dev/video" + camNum + " ! video/x-raw, format=UYVY, width=1920, height=1080 ! videoconvert ! video/x-raw, format=BGR ! appsink");
        
    }
   

    //creating simultaneous thread of each camera and pushing back run until all threads created


    for (int i=0; i < numCams; i++){
        threads.push_back(std::thread(video_capture,i));
        
    }
    //joining all threads simultaneously
    for(auto& thread : threads){
        thread.join();
    }
    // condition.notify_one();
 
    cout << "writing is done" << endl;
    return 0;

}
