#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace cv;

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


int main()
{
    std::cout << "VISION CORE: INITIALIZED\n";

    if(!cap.open(0))
    {
        std::cout << "ERROR: CANNOT ACCESS CAMERA\n VISION CORE: STOPPED\n";

        return 0;
    }

    while (run == true)
    {
        Mat src, blurred, filtered, canny, contour;
        Mat tempProcessed, drawing;

        cap >> src;

        //Checks if frame is empty
        if (frame.empty())
        {
            std::cout << "ERROR: CANNOT ACCESS FRAME\n VISION CORE: STOPPED\n";
            return 0;
        }

        assert(src.type() == CV_8UC3);

        //Blur
        GaussianBlur(src, blurred, Size(15,15), 0, 0);

        //Filter Color
        cvtColor(blurred, tempProcessed, CV_BGR2HSV);
        inRange(tempProsessed, Scalar(minH, minS, minV), Scalar(maxH, maxS, maxV), filtered);
        
        //Contour
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        cvtColor(filtered, tempProcessed, CV_HSV2RGB;
        cvtColor(tempProcessed, tempProcessed, CV_RGB2GRAY);

        Canny(tempProcessed, canny, cannyThresh, cannyThresh*2, 3);
        findContours(canny, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
                      //Dont need to draw? contours is a vector


        drawing = Mat::zeros(src.rows, src.cols, CV_8UC3);


        
        

    }

    std::cout << "VISION CORE: STOPPED\n";

    return 0;
}

void loadGlobalVar()
{
    
}
