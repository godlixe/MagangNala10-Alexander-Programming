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

/* Fungsi komparator untuk sorting contours */
bool compareContourAreas (vector<Point> contour1, vector<Point> contour2 ) {
    double i = fabs( contourArea(Mat(contour1)) );
    double j = fabs( contourArea(Mat(contour2)) );
    return ( i < j );
}

/* Variabel koordinat dan object setpoint */
int x = 640/2; //x titik pusat
int y = 480/2; //y titik pusat
int mx1 = 0, mx2 = 0, my1 = 0, my2 = 0, pmx = 0, pmy = 0; //x dan y bola merah
int hx1 = 0, hx2 = 0, hy1 = 0, hy2 = 0, phx = 0, phy = 0; //x dan y bola hijau
int kiri = 0, kanan = 0, lurus = 0; //variabel error
bool s;
Rect mb, hb; //bounding box merah dan hijau;
int r = 20;

/* PID */
int err_kiri[3], err_kanan[3];
int e_accum_kiri = 0, e_accum_kanan = 0;
int derivative;

int sumAv(int arr[]){
    int sum;
    sum = (arr[0] + arr[1] + arr[2]);
    return sum;
}

void inputError(int *arr, int value){
    arr[2] = arr[1];
    arr[1] = arr[0];
    arr[0] = value;
}

int main()
{

    int ero = 0, dil = 0;
    Mat frame, fullImageHSV, fullImageHSV2, mask, mask2, frame_threshold, frame_threshold2;
    VideoCapture cap(-1);
    namedWindow("My Window", WINDOW_NORMAL);
    namedWindow("Trackbar PID", WINDOW_NORMAL);

    //Trackbar HSV
    int glh=0, gls=0, glv=0, rlh=0, rls=0, rlv=0, uh=180, us=255, uv=255;
    createTrackbar("H_Merah", "My Window", &glh, uh);
    createTrackbar("S_Merah", "My Window", &gls, us);
    createTrackbar("V_Merah", "My Window", &glv, uv);
    createTrackbar("H_Hijau", "My Window", &rlh, uh);
    createTrackbar("S_Hijau", "My Window", &rls, us);
    createTrackbar("V_Hijau", "My Window", &rlv, uv);

    //Trackbar PID
    int Kp = 0, Ki = 0, Kd = 0;
    createTrackbar("P", "Trackbar PID", &Kp, 50);
    createTrackbar("I", "Trackbar PID", &Ki, 50);
    createTrackbar("D", "Trackbar PID", &Kd, 50);

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
                Rect mb = boundingRect(contours[contours.size()-1]);
                rectangle(frame, mb.tl(), mb.br(), Scalar(255,0,0), 3);
                mx1 = mb.x;
                mx2 = mb.x + mb.width;
                my1 = mb.y;
                my2 = mb.y + mb.height;
                pmx = (mx1+mx2)/2;
                pmy = (my1+my2)/2;
            }

            vector <vector<Point>> contours2;
            findContours(frame_threshold2, contours2, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
            if(contours2.size()!=0){
            sort(contours2.begin(), contours2.end(), compareContourAreas);
                Rect hb = boundingRect(contours2[contours2.size()-1]);
                rectangle(frame, hb.tl(), hb.br(), Scalar(255,0,0), 3);
                    hx1 = hb.x;
                    hx2 = hb.x + hb.width;
                    hy1 = hb.y;
                    hy2 = hb.y + hb.height;
                    phx = (hx1+hx2)/2;
                    phy = (hy1+hy2)/2;
            }
            //Gambar
            Rect setpoint(220, 190, 200, 100);
            rectangle(frame, setpoint, Scalar(255, 255, 255), 3);
            circle(frame, Point(110, 240), r, Scalar(0, 0, 255), -1);
            circle(frame, Point(530, 240), r, Scalar(0, 255, 0), -1);
            
            kiri = 0;
            kanan = 0;
            lurus = 0;

            if(pmx+r > 220 && pmx+r < 420 && pmy > 190 && pmy < 290){
                kanan = pmx + (mx2-mx1)/2 - 110;
                kiri = 0;
            }
            else if(phx-r > 220 && phx-r < 420 && phy > 190 && phy < 290){
                kiri = phx - (hx2-hx1)/2 - 530;
                kanan = 0;
            }
            else{
                lurus = 1;
                kiri = 0;
                kanan = 0;
            }
            if(kiri){
                inputError(err_kiri, kiri);
                e_accum_kiri = sumAv(err_kiri);
                derivative = err_kiri[0] - err_kiri[1];
                kiri = Kp*err_kiri[0] + Ki*e_accum_kiri + Kd*derivative;
            }
            else if(kanan){
                inputError(err_kanan, kanan);
                e_accum_kanan = sumAv(err_kanan);
                derivative = err_kanan[0] - err_kanan[1];
                kanan = Kp*err_kanan[0] + Ki*e_accum_kanan + Kd*derivative;
            }
            char left[200];
            char right[200];
            sprintf(left, "kiri  = %d", kiri);
            sprintf(right, "kanan = %d", kanan);
            if(kiri){
                putText(frame, left, Point2f(15,50), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255));
                cout << kiri << endl;
            }
            else if(kanan){
                putText(frame, right, Point2f(15,100), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255));
                cout << kanan << endl;
            }
            else{
                putText(frame, "lurus", Point2f(15,100), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255));
                cout << "0" << endl;
            }
            //Output
            imshow("My Window", frame);
            imshow("My Window2", frame_threshold);
            imshow("My Window3", frame_threshold2);
            if( waitKey(10) == 27 ) break;
            contours.clear();
    }
    return 0;
}
