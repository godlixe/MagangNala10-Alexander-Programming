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

    Mat frame, fullImageHSV, fullImageHSV2, frame_threshold, frame_threshold2, mask;
    VideoCapture cap(-1);
    namedWindow("My Window", WINDOW_NORMAL);
    setWindowProperty("My Window", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);

    int glh=0, gls=0, glv=0, rlh=0, rls=0, rlv=0, uh=180, us=255, uv=255;
    createTrackbar("Green_LH", "My Window", &glh, uh);
    createTrackbar("Green_LS", "My Window", &gls, us);
    createTrackbar("Green_LV", "My Window", &glv, uv);
    createTrackbar("Red_LH", "My Window", &rlh, uh);
    createTrackbar("Red_LS", "My Window", &rls, us);
    createTrackbar("Red_LV", "My Window", &rlv, uv);
    
    if(!cap.open(0))
        return 0;
    for(;;)
    {
            cap >> frame;
            cvtColor(frame, fullImageHSV, COLOR_BGR2HSV);
            cvtColor(frame, fullImageHSV2, COLOR_BGR2HSV);
            if( frame.empty() ) break;
            inRange(fullImageHSV, Scalar(glh, gls, glv), Scalar(uh, us, uv), frame_threshold);
            inRange(fullImageHSV2, Scalar(rlh, rls, rlv), Scalar(uh, us, uv), frame_threshold2);
            //cvtColor(frame, frame, COLOR_BGR2HSV);
            //inRange(frame, greenLow, greenHigh, mask);
            
            vector <vector<Point>> contours;
            findContours(frame_threshold, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
            //drawContours(frame_threshold, contours, 1, (0,255,0), 3);
            for (size_t i = 0; i < contours.size(); i++){
                Rect boundRect = boundingRect(contours[i]);
                if(boundRect.area()>350 && (boundRect.width < 70|| boundRect.height<70)){
                    rectangle(frame, boundRect.tl(), boundRect.br(), (0,0,255), 2);
                }
            }
            vector <vector<Point>> contours2;
            findContours(frame_threshold2, contours2, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
            //drawContours(frame_threshold, contours, 1, (0,255,0), 3);
            for (size_t i = 0; i < contours2.size(); i++){
                Rect boundRect = boundingRect(contours2[i]);
                if(boundRect.area()>350 && (boundRect.width < 70|| boundRect.height<70)){
                    rectangle(frame, boundRect.tl(), boundRect.br(), (255,0,0), 2);
                }
            }
            //imshow("My Window", frame);
            imshow("My Window", frame);
            if( waitKey(10) >=0 ) break;
    }
    return 0;
}
