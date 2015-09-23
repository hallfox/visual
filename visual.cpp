#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <queue>

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
    int min, max;

    // Zero-out the new arrays
    for (int i = 0; i <= PIXEL_RANGE; i++) {
        hist[i] = 0;
        cdf[i] = 0;
    }
    // Computing the histogram
    for (int i = 0; i < size; i++) {
        hist[image->data[i]]++;
    }
    // Compute the cdf
    cdf[0] = hist[0];
    min = 0;
    max = hist[0];
    for (int i = 1; i <= PIXEL_RANGE; i++) {
        cdf[i] = cdf[i-1] + hist[i];
        if (min == 0 && cdf[i-1] != 0) {
            min = cdf[i-1];
        }
        if (cdf[i] != 0) {
            max = cdf[i];
        }
    }
    // Compute the equalized histogram
    for (int i = 0; i <= PIXEL_RANGE; i++) {
        hist[i] = cdf[i] * 255 / size;
    }
    // Finally, change the image to reflect the new histogram
    for (int i = 0; i < size; i++) {
        image->data[i] = hist[image->data[i]];
    }

}

void imAutoThresh(Mat *image) {
    // Find average pixel, use that as threshold
    // Iterate through picture, cutting off insufficient pixels
    int size = imSize(image);
    int tpi = 0; // Total pixel intensity
    for (int i = 0; i < size; i++) {
        tpi += image->data[i];
    }

    int thresh = tpi / size;
    cout << "The threshold is: " << thresh << endl;

    for (int i = 0; i < size; i++) {
        image->data[i] = image->data[i] < thresh ? 0 : 255;
    }
}

void imRegionDetect(Mat *image) {
    imAutoThresh(image); // Need to make sure we've pulled out the noisy bits
    int shade = 0;
    int size = imSize(image);
    int width = image->size().width, height = image->size().height;
    queue<int> toVisit;
    vector<bool> visited(size, false);

    for (int i = 0; i < size; i++) {
        if (!visited[i] && image->data[i] == 255) {
            cout << "New region detected: " << i << endl;
            toVisit.push(i); // New region found
            visited[i] = true;
            shade = (shade + 50) % 255;
        }
        while (!toVisit.empty()) {
            int pix = toVisit.front(); toVisit.pop();
            image->data[pix] = shade;
            int left = pix-1, right = pix+1,
                up = pix-width, down = pix+width;
            if (!visited[left] && pix % width != 0 && image->data[left] == 255) {
                toVisit.push(left);
                visited[left] = true;
            }
            if (!visited[right] && right % width != 0 && image->data[right] == 255) {
                toVisit.push(right);
                visited[right] = true;
            }
            if (!visited[up] && up >= 0 && image->data[up] == 255) {
                toVisit.push(up);
                visited[up] = true;
            }
            if (!visited[down] && down < size && image->data[down] == 255) {
                toVisit.push(down);
                visited[down] = true;
            }
        }
    }
}

int main(int argc, char **argv) {
    if(argc != 2) {
        cout << "USAGE: skeleton <input file path>" << endl;
        return -1;
    }

    //Load two copies of the image. One to leave as the original, and one to be modified.
    //Done for display purposes only
    Mat original_image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    Mat eqImage = original_image.clone(), 
        negImage = original_image.clone(),
        threshImage = original_image.clone(),
        regionImage = original_image.clone();

    //Create a pointer so that we can quickly toggle between which image is being displayed
    Mat *image = &original_image;

    //Check that the images loaded
    if(!original_image.data || !eqImage.data || !negImage.data) {
        cout << "ERROR: Could not load image data." << endl;
        return -1;
    }
    
    // Apply our custom filters
    imEqualize(&eqImage);
    imNegative(&negImage);
    imAutoThresh(&threshImage);
    imRegionDetect(&regionImage);

    //Create the display window
    namedWindow("Assignment 1");


    //Display loop
    bool loop = true;
    while(loop) {
        imshow("Assignment 1", *image);

        char c = cvWaitKey(15);
        switch(c) {
            case 27:  //Exit display loop if ESC is pressed
                loop = false;
                break;
            case ' ': // Go back to the original image
                image = &original_image;
                break;
            case '=':
                image = &eqImage;
                break;
            case '-':
                image = &negImage;
                break;
            case 'b':
                image = &threshImage;
                break;
            case 'r':
                image = &regionImage;
                break;
            default:
            break;
        }
    }
    imwrite("neg.tif", negImage);
    imwrite("equal.tif", eqImage);
    imwrite("binary.tif", threshImage);
    imwrite("region.tif", regionImage);
}
