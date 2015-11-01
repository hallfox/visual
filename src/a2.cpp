#include "visual.hpp"

/****** ASSIGNMENT 2 FUNCTIONS ********/

using namespace cv;
using namespace std;

Mat applyMask(const Mat &image, int *mask, int maskSize) {
    // Applies generic 3x3 mask on image
    Mat blur = image.clone();
    for (int row = maskSize / 2; row < image.rows - maskSize / 2; row++) {
        for (int col = maskSize / 2; col < image.cols - maskSize / 2; col++) {
            // Gets the value of an applied mask of size 3x3
            short val = 0;
            for (int m = 0; m < maskSize; m++) {
                for (int n = 0; n < maskSize; n++) {
                    int r = row+m - 1,
                        c = col+n - 1;
                    val += mask[maskSize*m + n] * blur.at<float>(r, c);
                }
            }
            blur.at<float>(row, col) = val;
        }
    }
    double mn, mx;
    minMaxLoc(blur, &mn, &mx);
    cout << "Max: " << mx << endl;
    cout << "Min: " << mn << endl;
    cout << "Scale factor: " << 1 / (mx) << endl;

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
    pow(gx, 2, gx);
    pow(gy, 2, gy);
    sqrt(gx + gy, image);
    double min, max;
    minMaxLoc(image, &min, &max);
    image = (image - min) / max;
}

void imLogA(Mat &image) {
    int logA[] = {
         0,  1,  1,  1,  1,  1,  0,
         1,  2,  2,  1,  2,  2,  1,
         1,  2,  0, -1,  0,  2,  1,
         1,  1, -1, -5, -1,  1,  1,
         1,  2,  0, -1,  0,  2,  1,
         1,  2,  2,  1,  2,  2,  1,
         0,  1,  1,  1,  1,  1,  0
    };
    image -= applyMask(image, logA, 7);
}

void imLogB(Mat &image) {
    int logB[] = {
         4,  3,  2,  1,  0,  0,  0,  1,  2,  3,  4,
         3,  2,  1,  0,  0, -1,  0,  0,  1,  2,  3,
         2,  1,  0, -1, -1, -2, -1, -1,  0,  1,  2,
         1,  0,  0, -1, -2, -3, -2, -1,  0,  0,  1,
         0,  0, -1, -2, -3, -3, -3, -2, -1,  0,  0,
         0, -1, -2, -3, -3, -5, -3, -3, -2, -1,  0,
         0,  0, -1, -2, -3, -3, -3, -2, -1,  0,  0,
         1,  0,  0, -1, -2, -3, -2, -1,  0,  0,  1,
         2,  1,  0, -1, -1, -2, -1, -1,  0,  1,  2,
         3,  2,  1,  0,  0, -1,  0,  0,  1,  2,  3,
         4,  3,  2,  1,  0,  0,  0,  1,  2,  3,  4
    };
    image -= applyMask(image, logB, 11);
}
