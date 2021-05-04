#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

Scalar greenLow = Scalar(40, 40, 40);
Scalar greenHigh = Scalar(70, 255, 255);
int main()
{

    Mat frame, fullImageHSV,frame_threshold, mask;
    VideoCapture cap(-1);
    namedWindow("My Window", 1);

    int lh=0, ls=0, lv=0, uh=180, us=255, uv=255;
    createTrackbar("LH", "My Window", &lh, uh);
    createTrackbar("LS", "My Window", &ls, us);
    createTrackbar("LV", "My Window", &lv, uv);
    createTrackbar("UH", "My Window", &uh, uh);
    createTrackbar("US", "My Window", &us, us);
    createTrackbar("UV", "My Window", &uv, uv);
    
    if(!cap.open(0))
        return 0;
    for(;;)
    {
            cap >> frame;
            cvtColor(frame, fullImageHSV, COLOR_BGR2HSV);
            if( frame.empty() ) break;
            //inRange(fullImageHSV, Scalar(lh, ls, lv), Scalar(uh, us, uv), frame_threshold);
            //cvtColor(frame, frame, COLOR_BGR2HSV);
            inRange(frame, greenLow, greenHigh, mask);
            vector <vector<Point>> contours;
            findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
            drawContours(frame, contours, 1, (0,255,0), 3);
            imshow("My Window", frame);
            //imshow("My Window", frame_threshold);
            if( waitKey(10) >=0 ) break;
    }
    return 0;
}