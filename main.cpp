//FRC 4669 TX1
//Resource: http://docs.opencv.org/2.4/doc/tutorials/introduction/load_save_image/load_save_image.html
//Resource: http://answers.opencv.org/question/1/how-can-i-get-frames-from-my-webcam/
//Resource: http://www.jetsonhacks.com/2015/12/29/gpio-interfacing-nvidia-jetson-tx1/
#include <iostream>
#include <opencv2/core/core.hpp>
using namespace std;

//Prototypes
void startup();
void imgCapture(int frame);
void light(bool status);
void compare();

string trigger = "";

int main()
{
    startup();
    
    while (true)
    {
        if (trigger == "cap")
        {
            imgCapture(1);
            light(true);
            imgCapture(2);
            light(false);
            compare();
        }
        else if (trigger == "kill")
        {
            cout << "CV STOPPED";
            break;
        }

    }
    
    return 0;
}
