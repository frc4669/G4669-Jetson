/**
* Galileo Robotics Computer Vision
* @author  Kenny Ngo
* @date    3/6/2017
* @version 2.0
*/

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
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

Mat src, blurred, blurredHSV, filtered, filteredGray, canny, threshold_output, scaled;
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

    loadGlobalVar();

    while (run == true)
    {
        if( waitKey(1) == 27 ) 
            break; // stop capturing by holding ESC 

        //cap >> src; **********************CHANGE LATER
        src = imread("dark.jpg", 1);

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
        inRange(blurredHSV, Scalar(minH, minS, minV), Scalar(maxH, maxS, maxV), filtered);

        //Contour
        contDrawing = Mat::zeros(src.rows, src.cols, CV_8UC3);

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
            rectangle( contDrawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
        }

        //Corner Detection
        cornerDrawing = Mat::zeros( contDrawing.size(), CV_32FC1 );
  
        cornerHarris(contDrawing, tempProcessed, blockSize, apertureSize, k, BORDER_DEFAULT);
        
        normalize(tempProcessed, normal, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
        convertScaleAbs(normal, normScaled);

        for (int x = 0; x < normal.rows; x++)
        {
            for (int y = 0; y < normal.cols; y++)
            {
                if ((int) normal.at<float>(x,y) > cornerThresh)
                    circle(normScaled, Point(y,x), 5, Scalar(0), 2, 8, 0);
            }
        }


        imshow("Drawing", normScaled); //Keep uncommented in development to avoid videoio error
    }

    cout << "VISION CORE: STOPPED";

    return 0;
}

void loadGlobalVar()
{
    ifstream config;
    config.open("conf_file.txt");

    if (config)
    {
        config >> minH;
        config >> minS;
        config >> minV;
        config >> maxH;
        config >> maxS;
        config >> maxV;
        config >> cannyThresh;
        config >> thresholdThresh;
        config >> cornerThresh;

        config.close();
    }
    else
        cout << "VISION ERROR: CANNOT ACCESS PRE-SET VARIABLE, VARIABLES SET TO DEFAULT VALUES\n"; 

}
