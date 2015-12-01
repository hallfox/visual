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
    Mat original_image = imread(argv[1]);//, IMREAD_GRAYSCALE);
    Mat modified_image = original_image.clone();

    //Check that the images loaded
    if(!original_image.data || !modified_image.data) {
        cout << "ERROR: Could not load image data." << endl;
        return -1;
    }

    int defSliderVal = 0;

    //Create the display window
    namedWindow("Visual");
    createTrackbar("Threshold", "Visual", &defSliderVal, 255);

    //Display loop
    bool loop = true;
    int autoThresh;
    Mat chans[3];
    Mat transformed;
    Mat mask;
    Mat votes = Mat::zeros(200, 200, CV_8U);
    while(loop) {
      imshow("Visual", modified_image);

      char c = cvWaitKey(15);
      switch(c) {
      case 27:  //Exit display loop if ESC is pressed
        loop = false;
        break;
      case ' ': // Go back to the original image
        original_image.copyTo(modified_image);
        setTrackbarPos("Threshold", "Visual", 0);
        break;
      case '=':
        imEqualize(&modified_image);
        break;
      case '-':
        imNegative(&modified_image);
        break;
      case 'b':
        autoThresh = imOtsuBinary(modified_image);
        setTrackbarPos("Threshold", "Visual", autoThresh);
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
        imThresh(&modified_image, getTrackbarPos("Threshold", "Visual"));
        break;
      case 'u':
        imSharpen(modified_image);
        break;
      case 's':
        imToHSI(modified_image);
        modified_image = hsiToGs(modified_image);
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
        // split(modified_image, chans);
        // modified_image = chans[2];
        break;
      case 'H':
        imFromHSI(modified_image);
        break;
      case 'd':
        original_image.copyTo(modified_image);
        imToHSI(modified_image);
        modified_image = imDct(modified_image, 3);
        split(modified_image, chans);
        modified_image = chans[0];
        break;
      case 'D':
        original_image.copyTo(modified_image);
        imToHSI(modified_image);
        modified_image = imIdct(imDct(modified_image, 3));
        split(modified_image, chans);
        modified_image = chans[2];
        break;
      case 'e':
        original_image.copyTo(modified_image);
        imToHSI(modified_image);
        modified_image = imDct(modified_image, 1);
        imFromHSI(modified_image);
        break;
      case 'E':
        original_image.copyTo(modified_image);
        imToHSI(modified_image);
        modified_image = imIdct(imDct(modified_image, 1));
        split(modified_image, chans);
        modified_image = chans[2];
        break;
      case 'x':
        original_image.copyTo(modified_image);
        votes = imLineDetect(modified_image);
        break;
      case 'X':
        original_image.copyTo(modified_image);
        votes = imCircDetect(modified_image);
        break;
      case 'g':
        imGray(modified_image);
        break;
      case 'm':
        modified_image = imDilate(modified_image, makeCross(2));
        break;
      case 'M':
        modified_image = imErode(modified_image, makeCross(2));
        break;
      case '.':
        imGray(modified_image);
        modified_image.copyTo(mask);
        imOtsuBinary(mask);
        mask = imOpen(mask, makeCross(1));
        mask = imOpen(mask, makeCross(3));
        mask = imClose(mask, makeSquare(1));
        // mask = imOpen(mask, makeCross(1));
        mask = imClose(mask, makeCross(1));
        modified_image = imFilterMask(modified_image, mask);
        break;
      case 9: // Tab
        modified_image = votes;
        break;
      case '1':
        imGray(modified_image);
        imManualTrain(modified_image, modified_image);
        break;
      case '2':
        imGray(modified_image);
        imNNTrain(modified_image, modified_image);
        break;
      case '3':
        imGray(modified_image);
        imNNCopy(modified_image, modified_image);
        break;
      case '4':
        imGray(modified_image);
        imNNAverage(modified_image, modified_image);
        break;
      case '5':
        imGray(modified_image);
        imNNClassAverage(modified_image, modified_image);
        break;
      case '6':
        imGray(modified_image);
        imManualTest(modified_image, modified_image);
        break;
      case '8':
        imGray(modified_image);
        imKMeans(modified_image, modified_image);
        break;
      default:
        break;
        }
    }

    // Cleanup
    destroyAllWindows();
}
