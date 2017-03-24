/**
* Galileo Robotics Computer Vision
* @author  Kenny Ngo
* @date    3/6/2017
* @version 2.5
*/

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <networktables/NetworkTable.h>
#include <iostream>
#include <fstream>
#include <math.h>
using namespace cv;
using namespace std;

//Prototypes
void loadGlobalVar();

//Global Variables
String conf_file = "SI.txt";

bool run = true;
VideoCapture cap;

int minH = 44;
int minS = 0;
int minV = 226;

int maxH = 84;
int maxS = 67;
int maxV = 255;

int cannyThresh = 100;
int thresholdThresh = 100; //Not sure if this should be the same var as cannyThresh
int cornerThresh = 235;

int blockSize = 2;
int apertureSize = 3;
double k = 0.04;

int maxX = -1;
int minX = 10000;
int centerX = 0;
int width = 0;
double dist;
double angle;

int contour_length_threshold = 10;

Mat src, blurred, blurredHSV, filtered, filteredGray, canny, threshold_output, scaled, contGray;
Mat tempProcessed, contDrawing, cornerDrawing, normal, normScaled;

vector<vector<Point> > contours;
vector<Vec4i> hierarchy;


int main()
{
    cout << "VISION CORE: INITIALIZED\n";

    NetworkTable::SetClientMode();
    NetworkTable::SetIPAddress("10.46.69.2");
    NetworkTable::Initialize();
    shared_ptr<NetworkTable> myTable = NetworkTable::GetTable("vision");

    cout << "VISION CORE: NETWORK TABLES INITIALIZED\n";
    
    loadGlobalVar();

    //snamedWindow("Color Filtered", 1);

    //createTrackbar("Min H", "Color Filtered", &minH, 179);
    //createTrackbar("Max H", "Color Filtered", &maxH, 179);
    
    //createTrackbar("Min S", "Color Filtered", &minS, 255);
    //createTrackbar("Max S", "Color Filtered", &maxS, 255);

    //createTrackbar("Min V", "Color Filtered", &minV, 255);
    //createTrackbar("Max V", "Color Filtered", &maxV, 255);

    if(!cap.open(0))
    {
        cout << "VISION ERROR: CANNOT ACCESS CAMERA\n" << "VISION CORE: STOPPED\n";

        return 0;
    }

    cap.set(CV_CAP_PROP_BUFFERSIZE, 0);
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
    //cap.set(CV_CAP_PROP_EXPOSURE, -100);

    while (true)
    {
        if(waitKey(1) == 27 || run == false) 
            break; // stop capturing by holding ESC 

        //src = cap.grab();
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

        //Filter out small countour
        for(vector<vector<Point> >::iterator it = contours.begin(); it!=contours.end();)
        {
            if (it->size()<contour_length_threshold)
                it = contours.erase(it);
            else
                ++it;
        }

        vector<cv::Point> merged_contour_points;
        for (int i = 0; i < contours_poly.size(); i++) 
        {
            for (int j = 0; j < contours_poly[i].size(); j++) 
            {
                merged_contour_points.push_back(contours_poly[i][j]);
            }
        }


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

        //imshow("contDrawing", contDrawing); ///////////////////////////GET RID OF

        //Corner Detection
        cvtColor(contDrawing, contGray, CV_RGB2GRAY);

        cornerDrawing = Mat::zeros( contDrawing.size(), CV_32FC1 );

        cornerHarris( contGray, cornerDrawing, blockSize, apertureSize, k, BORDER_DEFAULT );
        normalize(cornerDrawing, normal, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );

        //convertScaleAbs(normal, normScaled);
        
        maxX = -1;
        minX = 10000;

        for (int y = 0; y < normal.rows; y++)
        {
            for (int x = 0; x < normal.cols; x++)
            {
                if ((int) normal.at<float>(y,x) > cornerThresh)
                {
                    //circle(normal, Point(x,y), 5, Scalar(0), 2, 8, 0);
                    cout << x << " " << y << endl;
                    if (x > maxX)
                        maxX = x;
                    if (x < minX)
                        minX = x;
                }
            }
        }

        centerX = (maxX + minX) / 2.0;
        width = maxX - minX;
        cout << centerX << " " << width << endl;
        cout << "MAX: " << maxX << " MIN: " << minX << endl;
        cout << "CENTER: " << centerX << endl;
        cout << "WIDTH: " << width << endl;

        //dist = -0.15 * width + 62.9;
        //angle = atan((centerX - (640.0/2.0)) / dist);

        //cout << "Distance: " << dist << " Angle: " << angle << endl;

        myTable->PutNumber("centerX", centerX);
        myTable->PutNumber("width", width);

        //imshow("Source", src);
        //imshow("Color Filtered", filtered);
        //imshow("Drawing", normal); //Keep uncommented in development to avoid videoio error
    }

    NetworkTable::Shutdown();
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
        config >> maxH;
        config >> minS;
        config >> maxS;
        config >> minV;
        config >> maxV;
        config >> cannyThresh;
        config >> thresholdThresh;
        config >> cornerThresh;
        config >> contour_length_threshold;

        cout << "VISION CORE: VARIABLE PRESET LOADING SUCCESSFUL\n";

        config.close();
    }
    else
        cout << "VISION ERROR: CANNOT ACCESS PRE-SET VARIABLE, VARIABLES SET TO DEFAULT VALUES\n"; 

}
