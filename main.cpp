//FRC 4669 TX1
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