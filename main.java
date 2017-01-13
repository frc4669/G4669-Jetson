#FRC 4669 JETSON TX1 CODE
import org.opencv.core.*;
import org.opencv.highgui.VideoCapture;

/* TO-DO LIST:
Network tables: Create trigger string variable
Create imgCapture method
Create light method
Create compare method
*/

public class  main {
    public static void main(String[] args) {
        startUp();

        while (true) {
            if (trigger.equals("cap") {
                imgCapture(1);
                light(true);
                imgCapture(2);
                light(false);
                compare();
            }
            else if (trigger.equals("kill")
                break;
    }


    public void startUp() {
        //CAMERA INITIALIZATION
        //Thread.sleep(1000);           DONT KNOW IF NECESSARY, UNCOMMENT IF CAMERA DOES NOT START
        VideoCapture camera = new VideoCapture(0);

    }

}

