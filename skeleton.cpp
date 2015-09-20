#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include <iostream>

#define PIXEL_RANGE 255

using namespace std;
using namespace cv;

size_t imSize(Mat *image) {
    return image->rows * image->step;
}

void imNegative(Mat *image) {
    size_t size = imSize(image);
    for (int i = 0; i < size; i++) {
        image->data[i] = PIXEL_RANGE - image->data[i];
    }
}

void imEqualize(Mat *image) {
    size_t size = imSize(image);
    int hist[PIXEL_RANGE+1];
    int cdf[PIXEL_RANGE+1];

    // Zero-out the new arrays
    for (int i = 0; i <= PIXEL_RANGE; i++) {
        hist[i] = 0;
        cdf[i] = 0;
    }
    // Computing the histogram
    for (int i = 0; i < size; i++) {
        hist[(unsigned int)image->data[i]]++;
    }
    // Compute the cdf
    cdf[0] = hist[0];
    for (int i = 1; i <= PIXEL_RANGE; i++) {
        cdf[i] = cdf[i-1] + hist[i];
    }
    // Compute the equalizer keys
    for (int i = 0; i <= PIXEL_RANGE; i++) {
        cdf[i] = (cdf[i] * PIXEL_RANGE) / size;
    }
    // Finally, use the cdf to transform the equalization
    for (int i = 0; i < size; i++) {
        image->data[i] = cdf[(unsigned int)image->data[i]];
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
  
  imEqualize(&modified_image);
  //for (int i = 0; i < imSize(image); i++) {
      //cout << (int)image->data[i] << " ";
  //}
  
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
