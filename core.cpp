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

int contourThresh = 450;


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
        Mat src, processed;

        cap >> src;

        //Checks if frame is empty
        if (frame.empty())
        {
            std::cout << "ERROR: CANNOT ACCESS FRAME\n VISION CORE: STOPPED\n";
            return 0;
        }

        assert(src.type() == CV_8UC3);

        //Blur
        GaussianBlur(src, processed, Size(15,15), 0, 0);

        //Filter Color
        cvtColor(processed, processed, CV_BGR2HSV);
        inRange(prosessed, Scalar(minH, minS, minV), Scalar(maxH, maxS, maxV), processed);
        
        //Contour
        

    }

    std::cout << "VISION CORE: STOPPED\n";

    return 0;
}

void loadGlobalVar()
{
    
}
