#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <queue>
#include <iostream>

#include "visual.hpp"


using namespace std;
using namespace cv;

int main(int argc, char **argv) {
    if(argc != 2) {
        cout << "USAGE: skeleton <input file path>" << endl;
        return -1;
    }

    //Load two copies of the image. One to leave as the original, and one to be modified.
    //Done for display purposes only
    Mat original_image = imread(argv[1]);
    Mat modified_image = original_image.clone();

    //Check that the images loaded
    if(!original_image.data || !modified_image.data) {
        cout << "ERROR: Could not load image data." << endl;
        return -1;
    }

    int defSliderVal = 0;

    //Create the display window
    namedWindow("Assignment 1");
    createTrackbar("Threshold", "Assignment 1", &defSliderVal, 255);

    //Display loop
    bool loop = true;
    int autoThresh;
    while(loop) {
        imshow("Assignment 1", modified_image);

        char c = cvWaitKey(15);
        switch(c) {
            case 27:  //Exit display loop if ESC is pressed
                loop = false;
                break;
            case ' ': // Go back to the original image
                original_image.copyTo(modified_image);
                setTrackbarPos("Threshold", "Assignment 1", 0);
                break;
            case '=':
                imEqualize(&modified_image);
                break;
            case '-':
                imNegative(&modified_image);
                break;
            case 'b':
                autoThresh = imAutoThresh(&modified_image);
                setTrackbarPos("Threshold", "Assignment 1", autoThresh);
                break;
            case 'r':
                imRegionDetect(&modified_image);
                break;
            case 'S':
                imwrite("out.tif", modified_image);
                cout << "Saved image as 'out.tif'.\n";
                break;
            case 't':
                original_image.copyTo(modified_image);
                imThresh(&modified_image, getTrackbarPos("Threshold", "Assignment 1"));
                break;
            case 'u':
                imSharpen(modified_image);
                break;
            case 's':
                imSobelEdge(modified_image);
                break;
            case 'l':
                imLogA(modified_image);
                break;
            case 'L':
                imLogB(modified_image);
                break;
            case 'h':
                imToHSI(modified_image);
            default:
            break;
        }
    }

    // Cleanup
    destroyAllWindows();
}
