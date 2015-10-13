#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <queue>

#include <iostream>

#define PIXEL_RANGE 255

using namespace std;
using namespace cv;

inline size_t imSize(Mat *image) {
    return image->rows * image->step;
}


/****** ASSIGNMENT 1 FUNCTIONS ********/

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

void imThresh(Mat *image, int thresh) {
    int size = imSize(image);
    for (int i = 0; i < size; i++) {
        image->data[i] = image->data[i] < thresh ? 0 : 255;
    }
}

int imAutoThresh(Mat *image) {
    // Find average pixel, use that as threshold
    // Iterate through picture, cutting off insufficient pixels
    int size = imSize(image);
    int tpi = 0; // Total pixel intensity
    for (int i = 0; i < size; i++) {
        tpi += image->data[i];
    }

    int thresh = tpi / size;
    imThresh(image, thresh);
    return thresh;
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
            toVisit.push(i); // New region found
            visited[i] = true;
            shade = (shade + 50) % 255;
        }
        while (!toVisit.empty()) {
            int pix = toVisit.front(); toVisit.pop();
            image->data[pix] = shade;
            int left = pix-1, right = pix+1,
                up = pix-width, down = pix+width;
            if (pix % width != 0 && !visited[left] && image->data[left] == 255) {
                toVisit.push(left);
                visited[left] = true;
            }
            if (right % width != 0 && !visited[right] && image->data[right] == 255) {
                toVisit.push(right);
                visited[right] = true;
            }
            if (up >= 0 && !visited[up] && image->data[up] == 255) {
                toVisit.push(up);
                visited[up] = true;
            }
            if (down < size && !visited[down] && image->data[down] == 255) {
                toVisit.push(down);
                visited[down] = true;
            }
        }
    }
}

/****** ASSIGNMENT 2 FUNCTIONS ********/

Mat applyMask(const Mat &image, int *mask, int maskSize) {
    // Applies generic 3x3 mask on image
    Mat blur;
    image.assignTo(blur, CV_32S);
    int size = imSize(&blur);
    for (int row = maskSize / 2; row < image.rows - maskSize / 2; row++) {
        for (int col = maskSize / 2; col < image.cols - maskSize / 2; col++) {
            // Gets the value of an applied mask of size 3x3
            short val = 0;
            for (int m = 0; m < maskSize; m++) {
                for (int n = 0; n < maskSize; n++) {
                    int r = row+m - 1,
                        c = col+n - 1;
                    val += mask[maskSize*m + n] * image.at<uchar>(r, c);
                }
            }
            blur.at<int>(row, col) = val;
        }
    }
    double mn, mx;
    minMaxLoc(blur, &mn, &mx);
    cout << "Max: " << mx << endl;
    cout << "Min: " << mn << endl;
    cout << "Scale factor: " << 255 / (mx-mn) << endl;
    blur -= mn;
    blur.convertTo(blur, CV_8U, 255 / (mx-mn));

    return blur;
}

void imSharpen(Mat &image) {
    // Sharpens image by applying the unsharpen mask on an image
    int unsharp[] = {-1, -1, -1, -1, 9, -1, -1, -1, -1};
    Mat mask = applyMask(image, unsharp, 3);
    image = 2*image - mask;
}

void imSobelEdge(Mat &image) {
    int sobelX[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    int sobelY[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    Mat gx = applyMask(image, sobelX, 3);
    Mat gy = applyMask(image, sobelY, 3);
    image = gx / 2 + gy / 2;
}

void imLogA(Mat &image) {
    int logA[] = {
         0, -1, -1, -1, -1, -1,  0,
        -1, -2, -2, -1, -2, -2, -1,
        -1, -2,  0, -1, -0, -2, -1,
        -1, -1,  1,  5,  1, -1, -1,
        -1, -2,  0,  1,  0, -2, -1,
        -1, -2, -2, -1, -2, -2, -1,
         0, -1, -1, -1, -1, -1,  0
    };
    image += applyMask(image, logA, 7);
}

void imLogB(Mat &image) {
    int logB[] = {
        -2, -2, -1,  0,  0,  0,  0,  0, -1, -2, -2,
        -2, -1,  0,  1,  1,  2,  1,  1,  0, -1, -2,
        -1,  0,  1,  2,  3,  4,  3,  2,  1,  0, -1,
         0,  1,  2,  4,  5,  6,  5,  4,  2,  1,  0,
         0,  1,  3,  5,  7,  7,  7,  5,  3,  1,  0,
         0,  2,  4,  6,  7,  8,  7,  6,  4,  2,  0,
         0,  1,  3,  5,  7,  7,  7,  5,  3,  1,  0,
         0,  1,  2,  4,  5,  6,  5,  4,  2,  1,  0,
        -1,  0,  1,  2,  3,  4,  3,  2,  1,  0, -1,
        -2, -1,  0,  1,  1,  2,  1,  1,  0, -1, -2,
        -2, -2, -1,  0,  0,  0,  0,  0, -1, -2, -2
    };
    image = applyMask(image, logB, 11);
}

int main(int argc, char **argv) {
    if(argc != 2) {
        cout << "USAGE: skeleton <input file path>" << endl;
        return -1;
    }

    //Load two copies of the image. One to leave as the original, and one to be modified.
    //Done for display purposes only
    Mat original_image = imread(argv[1], IMREAD_GRAYSCALE);
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
            default:
            break;
        }
    }

    // Cleanup
    destroyAllWindows();
}
