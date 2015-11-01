#include "visual.hpp"

/****** ASSIGNMENT 1 FUNCTIONS ********/

using namespace cv;
using namespace std;

inline int imSize(Mat *image) {
  return image->rows * image->step;
}

void imNegative(Mat *image) {
    int size = imSize(image);
    for (int i = 0; i < size; i++) {
        image->data[i] = PIXEL_RANGE - image->data[i];
    }
}

void imEqualize(Mat *image) {
    int size = imSize(image);
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
