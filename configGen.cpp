/**
* Galileo Robotics Computer Vision Configuration Generator
* @author  Kenny Ngo
* @date    3/10/2017
* @version 1.0
*/

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
using namespace cv;
using namespace std;

//Prototypes
void processing();

//Global Variables
String conf_file = "102.txt";

bool run = true;
VideoCapture cap;

int minH = 0;
int minS = 0;
int minV = 0;

int maxH = 179;
int maxS = 255;
int maxV = 255;

int cannyThresh = 450;
int thresholdThresh = 254; //Not sure if this should be the same var as cannyThresh
int cornerThresh = 235;

int blockSize = 2;
int apertureSize = 3;
double k = 0.04;

Mat src, blurred, blurredHSV, filtered, filteredGray, bitwise, bitwise_gray, canny, threshold_output, scaled, contGray;
Mat tempProcessed, contDrawing, cornerDrawing, normal, normScaled;

vector<vector<Point> > contours;
vector<Vec4i> hierarchy;

int main()
{
    cout << "VISION CORE: INITIALIZED\n";

    if(!cap.open(0))
    {
        cout << "VISION ERROR: CANNOT ACCESS CAMERA\n" << "VISION CORE: STOPPED\n";

        return 0;
    }

    namedWindow("Source", 1);
    namedWindow("Color Filtered", 1);
    namedWindow("Canny", 1);
    namedWindow("Drawing", 1);

    createTrackbar("Min H", "Color Filtered", &minH, 179);
    createTrackbar("Max H", "Color Filtered", &maxH, 179);
    
    createTrackbar("Min S", "Color Filtered", &minS, 255);
    createTrackbar("Max S", "Color Filtered", &maxS, 255);

    createTrackbar("Min V", "Color Filtered", &minV, 255);
    createTrackbar("Max V", "Color Filtered", &maxV, 255);

    createTrackbar("Canny Thresh", "Canny", &cannyThresh, 800);
    createTrackbar("Corner Thresh", "Drawing", &cornerThresh, 255);

    processing();

    cout << "VISION CORE: STOPPED\n";

    return 0;
}

void processing()
{
    while (run == true)
    {
        if( waitKey(1) == 27 ) 
            break; // stop capturing by holding ESC 

        //cap >> src; **********************CHANGE LATER
        src = imread("dark.jpg", 1);

        //Checks if frame is empty
        if (src.empty())
        {
            cout << "VISION ERROR: CANNOT ACCESS FRAME\n";
            break;
        }

        //Blur
        GaussianBlur(src, blurred, Size(15,15), 0, 0);

        //Filter Color
        assert(blurred.type() == CV_8UC3);
        cvtColor(blurred, blurredHSV, CV_BGR2HSV);
        inRange(blurredHSV, Scalar(minH, minS, minV), Scalar(maxH, maxS, maxV), filtered);
        bitwise_and(blurredHSV, blurredHSV, bitwise, filtered = filtered);

        //Contour
        contDrawing = Mat::zeros(src.rows, src.cols, CV_8UC3);

        cvtColor(bitwise, bitwise_gray, CV_HSV2RGB);
        cvtColor(bitwise_gray, bitwise_gray, CV_RGB2GRAY);

        Canny(bitwise_gray, canny, cannyThresh, cannyThresh*2, 3);
        findContours( canny, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
        threshold(bitwise_gray, threshold_output, thresholdThresh, 255, THRESH_BINARY);

        //Bounding Rectangle
        vector<vector<Point> > contours_poly(contours.size());
        vector<Rect> boundRect(contours.size());

        //Filter out small countour

        for(vector<vector<Point> >::iterator it = contours.begin(); it!=contours.end();)
        {
            if (it->size()<contour_length_threshold)
                it = contours.erase(it);
            else
                ++it;
        }

        //Draw contour
        for( size_t i = 0; i < contours.size(); i++ )
        {
            approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
            boundRect[i] = boundingRect( Mat(contours_poly[i]) );
        }

        for( size_t i = 0; i< contours.size(); i++ )
        {
            Scalar color = Scalar( 0, 255, 0 );
            rectangle( contDrawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
        }

        //Corner Detection
        cvtColor(contDrawing, contGray, CV_RGB2GRAY);

        cornerDrawing = Mat::zeros( contDrawing.size(), CV_32FC1 );

        cornerHarris( contGray, cornerDrawing, blockSize, apertureSize, k, BORDER_DEFAULT );
        normalize(cornerDrawing, normal, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
        convertScaleAbs(normal, normScaled);

        for (int x = 0; x < normal.rows; x++)
        {
            for (int y = 0; y < normal.cols; y++)
            {
                if ((int) normal.at<float>(x,y) > cornerThresh)
                    circle(normScaled, Point(y,x), 5, Scalar(0), 2, 8, 0);
            }
        }

        imshow("Source", src);
        imshow("Color Filtered", bitwise);
        imshow("Drawing", normScaled);
        imshow("Canny", canny);
    }
}


