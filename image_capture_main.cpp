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
#include <opencv2/dnn/dnn.hpp>
// #include "zbar.h"

using namespace std;
using namespace cv;
// using namespace zbar;

namespace {

constexpr int kImageWidth = 1920;
constexpr int kImageHeight = 1080;
constexpr int kFramerate = 10;
constexpr int numCams = 6; 


std::string gstreamerpipeline[numCams];
std::string gstream_enc[numCams];

}  // namespace

// Image capture

void image_capture(int index){
    VideoCapture capture(gstreamerpipeline[index], cv::CAP_GSTREAMER);
    std::string camIndex = std::to_string(index);
    VideoWriter writer;
    cv::Mat frame;

    while (true){
        capture >> frame;
        cv::Mat frame_resized;
        resize(frame, frame_resized, Size(640,480));
        if (!capture.isOpened()) {
            cerr << "Could not open camera." << endl;
            exit(EXIT_FAILURE);
        } if (frame.empty()) {
            break;
        }
        
        imshow("Captured frame " + camIndex, frame_resized);
        namedWindow("Captured frame " + camIndex);

        int key = waitKey(1) & 0xff;
        // Stop the program on the ESC key.
        if (key == 27) {
            break;
        } else if (key == (int)('s')) {
            auto cppTimePoint = std::chrono::system_clock::now();
            auto cTimePoint = std::chrono::system_clock::to_time_t(cppTimePoint);
            string time_string(ctime(&cTimePoint));
            string image_path = "/home/motion/perception_experimental/prototyping/mitchell/mantis/mantis_image_test/mantis_test_image" + camIndex + time_string + ".png";
            std::cout << ctime(&cTimePoint) << std::endl;
            imwrite(image_path, frame);
            }
    }
    capture.release();
    // writer.release();


}
int main(int argc, char **argv) {
    int cam_idx = 0;

    if (argc == 2) {
        cam_idx = atoi(argv[1]);
    }
    
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
        threads.push_back(std::thread(image_capture,i));
        
    }
    //joining all threads simultaneously
    for(auto& thread : threads){
        thread.join();
    }
 
    cout << "writing is done" << endl;
   
        
    
    return 0;
}
