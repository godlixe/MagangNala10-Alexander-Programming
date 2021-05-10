#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

bool compareContourAreas (vector<Point> contour1, vector<Point> contour2 ) {
    double i = fabs( contourArea(cv::Mat(contour1)) );
    double j = fabs( contourArea(cv::Mat(contour2)) );
    return ( i < j );
}
int x = 640/2;
int y = 480/2;
int err = 0;
int main()
{

    int ero = 0, dil = 0;
    Mat frame, fullImageHSV, fullImageHSV2, mask, mask2, frame_threshold, frame_threshold2;
    VideoCapture cap(-1);
    namedWindow("My Window", WINDOW_NORMAL);

    int glh=0, gls=0, glv=0, rlh=0, rls=0, rlv=0, uh=180, us=255, uv=255;
    createTrackbar("Green_LH", "My Window", &glh, uh);
    createTrackbar("Green_LS", "My Window", &gls, us);
    createTrackbar("Green_LV", "My Window", &glv, uv);
    createTrackbar("ERO", "My Window", &ero, 10);
    createTrackbar("DIL", "My Window", &dil, 10);

    if(!cap.open(0))
        return 0;
    for(;;)
    {
            cap >> frame;
            cvtColor(frame, fullImageHSV, COLOR_BGR2HSV);
            if( frame.empty() ) break;
            inRange(fullImageHSV, Scalar(glh, gls, glv), Scalar(uh, us, uv), mask);
            //cvtColor(frame, frame, COLOR_BGR2HSV);
            //inRange(frame, greenLow, greenHigh, mask);
            Mat elementKernel = getStructuringElement(MORPH_RECT, Size(ero+1, ero+1), Point(ero,ero));
            Mat elementKernel2 = getStructuringElement(MORPH_RECT, Size(dil+1, dil+1), Point(dil,dil));
            erode(mask, frame_threshold, elementKernel, Point(-1,-1),1);
            dilate(mask, frame_threshold, elementKernel2, Point(-1,-1),1);
            

            vector <vector<Point>> contours;
            findContours(frame_threshold, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
            if(contours.size()!=0){
            sort(contours.begin(), contours.end(), compareContourAreas);
                Rect b = boundingRect(contours[contours.size()-1]);
                if(b.area()>1000){
                    rectangle(frame, b.tl(), b.br(), (0,0,255), 2);
                    if(b.x < x && b.x + b.width > x){
                        if(b.y < y && b.y + b.height > y){
                            err = 0;
                        }
                    }
                    else{
                        int midx = (2*b.x+b.width)/2;
                        int midy = (2*b.y+b.height)/2;
                        err = sqrt(pow((midx-x),2)+pow((midy-y),2));
                    }
                    char str[200];
                    char str2[200];
                    sprintf(str,"error = %d", err);
                    sprintf(str2,"x,y = (%d, %d)", b.x, b.y);
                    putText(frame, str, Point2f(100,50), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255));
                    putText(frame, str2, Point2f(100,100), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255));
                }
            }
            //imshow("My Window", frame);
            imshow("My Window", frame);
            imshow("My Window2", frame_threshold);
            //imshow("My Window", frame);
            if( waitKey(10) == 27 ) break;
            contours.clear();
    }
    return 0;
}
