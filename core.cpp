/**
* Galileo Robotics Computer Vision
* @author Kenny Ngo
* @date 3/6/2017
* @version 2.0
*/

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace cv;
using namespace std;

//Prototypes
void loadGlobalVar();

//Global Variables
bool run = true;
VideoCapture cap;

int minH = 44;
int minS = 0;
int minV = 226;

int maxH = 84;
int maxS = 67;
int maxV = 255;

int cannyThresh = 450;
int thresholdThresh = 450; //Not sure if this should be the same var as cannyThresh

int blockSize = 2;
int apertureSize = 3;
double k = 0.04;

Mat src, blurred, blurredHSV, filtered, filteredGray, canny, threshold_output;
Mat tempProcessed, drawing;

vector<vector<Point> > contours;
vector<Vec4i> hierarchy;



int main()
{
    cout << "VISION CORE: INITIALIZED\n";

    if(!cap.open(0))
    {
        cout << "ERROR: CANNOT ACCESS CAMERA\n VISION CORE: STOPPED\n";

        return 0;
    }

    while (run == true)
    {
        if( waitKey(1) == 27 ) 
            break; // stop capturing by holding ESC 

        //cap >> src; **********************CHANGE LATER
        src = imread("dark.jpg", 1);

        //Checks if frame is empty
        if (src.empty())
        {
            cout << "ERROR: CANNOT ACCESS FRAME\n VISION CORE: STOPPED\n";
            return 0;
        }

        //Blur
        GaussianBlur(src, blurred, Size(15,15), 0, 0);

        //Filter Color
        assert(blurred.type() == CV_8UC3);
        cvtColor(blurred, blurredHSV, CV_BGR2HSV);
        inRange(blurredHSV, Scalar(minH, minS, minV), Scalar(maxH, maxS, maxV), filtered);

        //Contour
        drawing = Mat::zeros(src.rows, src.cols, CV_8UC3);

        Canny(filtered, canny, cannyThresh, cannyThresh*2, 3);
        findContours( canny, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
        threshold(filtered, threshold_output, thresholdThresh, 255, THRESH_BINARY);

        //Bounding Rectangle
        vector<vector<Point> > contours_poly(contours.size());
        vector<Rect> boundRect(contours.size());

        for( size_t i = 0; i < contours.size(); i++ )
        {
            approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
            boundRect[i] = boundingRect( Mat(contours_poly[i]) );
        }

        for( size_t i = 0; i< contours.size(); i++ )
        {
            Scalar color = Scalar( 0, 255, 0 );
            rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
        }

        imshow("Drawing", drawing);
  
    }

    return 0;
}
