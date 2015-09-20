#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include <iostream>

using namespace std;
using namespace cv;

void imNegative(Mat *image) {
    size_t imSize = image->rows * image->step;
    for (int i = 0; i < imSize; i++) {
        image->data[i] = 255 - image->data[i];
    }
}

int main(int argc, char **argv) {
  if(argc != 2) {
    cout << "USAGE: skeleton <input file path>" << endl;
    return -1;
  }
  
  //Load two copies of the image. One to leave as the original, and one to be modified.
  //Done for display purposes only
  Mat original_image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  Mat modified_image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  
  //Create a pointer so that we can quickly toggle between which image is being displayed
  Mat *image = &original_image;
 
  //Check that the images loaded
  if(!original_image.data || !modified_image.data) {
    cout << "ERROR: Could not load image data." << endl;
    return -1;
  }
  
  //Create the display window
  namedWindow("Unix Sample Skeleton");
  
  imNegative(&modified_image);
  
  //Display loop
  bool loop = true;
  while(loop) {
    imshow("Unix Sample Skeleton", *image);
    
    char c = cvWaitKey(15);
    switch(c) {
      case 27:  //Exit display loop if ESC is pressed
        loop = false;
        break;
      case 32:  //Swap image pointer if space is pressed
        if(image == &original_image) image = &modified_image;
        else image = &original_image;
        break;
      default:
        break;
    }
  }
}
