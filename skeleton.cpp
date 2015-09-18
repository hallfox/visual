#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>


int main(int argc, char **argv) {
  using namespace std;
  using namespace cv;
  
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
  
  //Replace center third of the image with white
  //This can be replaced with whatever filtering you need to do.
  size_t offset1 = image->rows/3 * image->step;
  size_t offset2 = image->rows/3*2 * image->step;
  for(size_t i = offset1; i < offset2; i++)
    modified_image.data[i] = 255;
  
  //Display loop
  bool loop = true;
  while(loop) {
    imshow("Unix Sample Skeleton", *image);
    
    switch(cvWaitKey(15)) {
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
