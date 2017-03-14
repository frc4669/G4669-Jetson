/**
* Galileo Robotics Computer Vision
* @author  Kenny Ngo
* @date    3/6/2017
* @version 2.0
*/

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include <ntcore/ntcore_cpp.h>
#include <iostream>
#include <fstream>
using namespace cv;
using namespace std;

//Prototypes
void loadGlobalVar();

//Global Variables
String conf_file = "102.txt";

bool run = true;
VideoCapture cap;
//VideoCapture cap2;
//VideoCapture cap3;
//VideoCapture cap4;

int minH = 44;
int minS = 0;
int minV = 226;

int maxH = 84;
int maxS = 67;
int maxV = 255;

int cannyThresh = 450;
int thresholdThresh = 450; //Not sure if this should be the same var as cannyThresh
int cornerThresh = 235;

int blockSize = 2;
int apertureSize = 3;
double k = 0.04;

int contour_length_threshold = 40;

Mat src, blurred, blurredHSV, filtered, filteredGray, canny, threshold_output, scaled, contGray;
Mat tempProcessed, contDrawing, cornerDrawing, normal, normScaled;

vector<vector<Point> > contours;
vector<Vec4i> hierarchy;


int main()
{
    cout << "VISION CORE: INITIALIZED\n";

    if(!cap.open(2))
    {
        cout << "VISION ERROR: CANNOT ACCESS CAMERA\n" << "VISION CORE: STOPPED\n";

        return 0;
    }

    loadGlobalVar();

    while (run == true)
    {
        if( waitKey(1) == 27 ) 
            run = false; // stop capturing by holding ESC 

        cap >> src; //**********************CHANGE LATER
        //src = imread("dark.jpg", 1);

        //Checks if frame is empty
        if (src.empty())
        {
            cout << "VISION ERROR: CANNOT ACCESS FRAME\n" << "VISION CORE: STOPPED\n";
            return 0;
        }

        //Blur
        GaussianBlur(src, blurred, Size(15,15), 0, 0);

        //Filter Color
        assert(blurred.type() == CV_8UC3);
        cvtColor(blurred, blurredHSV, CV_BGR2HSV);
        inRange(blurredHSV, Scalar(minH, minS, minV), Scalar(maxH, maxS, maxV), tempProcessed);
        bitwise_and(blurred, blurred, filtered, tempProcessed = tempProcessed);

        cvtColor(filtered, filtered, CV_HSV2BGR);
        cvtColor(filtered, filtered, CV_BGR2GRAY);

        //Contour
        contDrawing = Mat::zeros(src.rows, src.cols, CV_8UC3);

        Canny(filtered, canny, cannyThresh, cannyThresh*2, 3);
        findContours( canny, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
        threshold(filtered, threshold_output, thresholdThresh, 255, THRESH_BINARY);

        //Bounding Rectangle
        vector<vector<Point> > contours_poly(contours.size());
        vector<Rect> boundRect(contours.size());

        /*Filter out small countour
        for(vector<vector<Point> >::iterator it = contours.begin(); it!=contours.end();)
        {
            if (it->size()<contour_length_threshold)
                it = contours.erase(it);
            else
                ++it;
        }
*/

        //Draw contour
        for(size_t i = 0; i < contours.size(); i++)
        {
            approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
            boundRect[i] = boundingRect( Mat(contours_poly[i]) );
        }

        for(size_t i = 0; i< contours.size(); i++)
        {
            Scalar color = Scalar( 0, 255, 0 );
            rectangle( contDrawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
        }

        imshow("contDrawing", contDrawing); ///////////////////////////GET RID OF

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
                {
                    circle(normScaled, Point(y,x), 5, Scalar(0), 2, 8, 0);
                    cout << x << " " << y << endl;
                }
            }
        }

        imshow("Source", src);
        imshow("Drawing", normScaled); //Keep uncommented in development to avoid videoio error
    }

    cout << "VISION CORE: STOPPED\n";

    return 0;
}

void loadGlobalVar()
{
    ifstream config;
    config.open(conf_file.c_str());

    if (config)
    {
        cout << "VISION CORE: VARIABLE PRESET LOADING STARTED\n";

        config >> minH;
        config >> minS;
        config >> minV;
        config >> maxH;
        config >> maxS;
        config >> maxV;
        config >> cannyThresh;
        config >> thresholdThresh;
        config >> cornerThresh;

        cout << "VISION CORE: VARIABLE PRESET LOADING SUCCESSFUL\n";

        config.close();
    }
    else
        cout << "VISION ERROR: CANNOT ACCESS PRE-SET VARIABLE, VARIABLES SET TO DEFAULT VALUES\n"; 

}
