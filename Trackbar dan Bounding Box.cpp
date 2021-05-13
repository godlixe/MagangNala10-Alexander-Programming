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

//Fungsi komparator untuk sorting contours
bool compareContourAreas (vector<Point> contour1, vector<Point> contour2 ) {
    double i = fabs( contourArea(Mat(contour1)) );
    double j = fabs( contourArea(Mat(contour2)) );
    return ( i < j );
}
int x = 640/2; //x titik pusat
int y = 480/2; //y titik pusat
int ox1 = 0, ox2 = 0, oy1 = 0, oy2 = 0; //x dan y objek luar
int ix1 = 0, ix2 = 0, iy1 = 0, iy2 = 0; //x dan y objek dalam
int xp = 0, yp = 0;
int errh = 0, errv = 0;   //error horizontal dan vertikal
int l = 0, d = 0, r = 0, u = 0;
bool s;
Rect ob, ib; //Rectangle outer & inner
int main()
{

    int ero = 0, dil = 0;
    Mat frame, fullImageHSV, fullImageHSV2, mask, mask2, frame_threshold, frame_threshold2;
    VideoCapture cap(-1);
    namedWindow("My Window", WINDOW_NORMAL);

    //Variabel trackbar & trackbar
    int glh=0, gls=0, glv=0, rlh=0, rls=0, rlv=0, uh=180, us=255, uv=255;
    createTrackbar("Outer_LH", "My Window", &glh, uh);
    createTrackbar("Outer_LS", "My Window", &gls, us);
    createTrackbar("Outer_LV", "My Window", &glv, uv);
    createTrackbar("Inner_LH", "My Window", &rlh, uh);
    createTrackbar("Inner_LS", "My Window", &rls, us);
    createTrackbar("Inner_LV", "My Window", &rlv, uv);
    //createTrackbar("ERO", "My Window", &ero, 10);
    //createTrackbar("DIL", "My Window", &dil, 10);

    if(!cap.open(0))
        return 0;
    for(;;)
    {
            cap >> frame;

            //BGR to HSV
            cvtColor(frame, fullImageHSV, COLOR_BGR2HSV);
            cvtColor(frame, fullImageHSV2, COLOR_BGR2HSV);

            if( frame.empty() ) break;
            line(frame, Point(0, y), Point(2*x, y), (255, 0, 0), 1);
            line(frame, Point(x, 0), Point(x, 2*y), (255, 0, 0), 1);
            //Fungsi trackbar HSV
            inRange(fullImageHSV, Scalar(glh, gls, glv), Scalar(uh, us, uv), mask);
            inRange(fullImageHSV2, Scalar(rlh, rls, rlv), Scalar(uh, us, uv), mask2);

            //Dilate & erode
            //Mat elementKernel = getStructuringElement(MORPH_RECT, Size(ero+1, ero+1), Point(ero,ero));
            Mat elementKernel2 = getStructuringElement(MORPH_RECT, Size(10, 10), Point(-1, -1));
            //erode(mask, frame_threshold, elementKernel, Point(-1,-1),1);
            dilate(mask, frame_threshold, elementKernel2, Point(-1,-1),1);
            dilate(mask2, frame_threshold2, elementKernel2, Point(-1,-1),1);
            
            vector <vector<Point>> contours;
            findContours(frame_threshold, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
            if(contours.size()!=0){
            sort(contours.begin(), contours.end(), compareContourAreas);
                Rect ob = boundingRect(contours[contours.size()-1]);
                ox1 = ob.x;
                ox2 = ob.x + ob.width;
                oy1 = ob.y;
                oy2 = ob.y + ob.height;
            }

            vector <vector<Point>> contours2;
            findContours(frame_threshold2, contours2, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
            for (size_t i = 0; i < contours2.size(); i++){
                Rect ib = boundingRect(contours2[i]);
                if(ib.area() > 1000){
                    ix1 = ib.x;
                    ix2 = ib.x + ib.width;
                    iy1 = ib.y;
                    iy2 = ib.y + ib.height;
                    if(ix1 > ox1 && ix2 < ox2){
                        if(iy1 > oy1 && iy2 < oy2){
                            rectangle(frame, ib.tl(), ib.br(), (0,0,255), 2);
                            s = 1;
                            xp = (ix1 + ix2)/2;
                            yp = (iy1 + iy2)/2;
                        }
                    }
                }
            }
            if((ix1 < x && ix2 > x)&&(iy1 < y && iy2 > y)){
                    u = 0;
                    d = 0;
                    l = 0;
                    r = 0;
            }
            else{
                if(xp < x){
                    l = xp-x;
                    if(y > iy1 && y < iy2){
                        u = 0;
                        d = 0;
                    }
                    r = 0;
                }
                if(xp > x){
                    r = xp-x;
                    if(y > iy1 && y < iy2){
                        u = 0;
                        d = 0;
                    }
                    l = 0;
                }
                if(yp > y){
                    d = yp-y;
                    if(x > ix1 && x < ix2){
                        l = 0;
                        r = 0;
                    }
                    u = 0;
                }
                if(yp < y){
                    u = yp-y;
                    if(x > ix1 && x < ix2){
                        l = 0;
                        r = 0;
                    }
                    d = 0;
                }
            }

            //Fungsi printing
            char up[200];
            char down[200];
            char left[200];
            char right[200];
            char kor[200];
            sprintf(up,"UP     = %d", abs(u));
            sprintf(down,"DOWN  = %d", abs(d));
            sprintf(left,"LEFT   = %d", abs(l));
            sprintf(right,"RIGHT  = %d", abs(r));
            sprintf(kor,"xp, yp  = %d, %d", xp, yp);
            putText(frame, up, Point2f(15,50), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255));
            putText(frame, down, Point2f(15,100), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255));
            putText(frame, left, Point2f(15,150), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255));
            putText(frame, right, Point2f(15,200), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255));
            putText(frame, kor, Point2f(15,250), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255));

            //Output
            imshow("My Window", frame);
            imshow("My Window2", frame_threshold);
            imshow("My Window3", frame_threshold2);
            if( waitKey(10) == 27 ) break;
            contours.clear();
    }
    return 0;
}
