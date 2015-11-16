#include "visual.hpp"

#include <vector>

#define GRAY 127
#define WHITE 255
#define BLACK 0
#define REGION_BOUND 2

std::vector<cv::Point2i> makeCross(int size) {
  std::vector<cv::Point2i> cross;
  cross.push_back(cv::Point2i(0, 0));
  for (int i = 1; i <= size; i++) {
    cross.push_back(cv::Point2i(0, i));
    cross.push_back(cv::Point2i(0, -i));
    cross.push_back(cv::Point2i(i, 0));
    cross.push_back(cv::Point2i(-i, 0));
  }
  return cross;
}

std::vector<cv::Point2i> makeSquare(int size) {
  std::vector<cv::Point2i> square;
  for (int x = -size; x <= size; x++) {
    for (int y = -size; y <= size; y++) {
      square.push_back(cv::Point2i(x, y));
    }
  }
  return square;
}

// Assumes grayscale image
int imOtsuBinary(cv::Mat& img) {
  std::vector<int> hist(256, 0);
  for (auto it = img.begin<uchar>(); it != img.end<uchar>(); it++) {
    hist[*it]++;
  }

  // Can you tell I stole this from Wikipedia?
  int totalPixels = img.rows * img.cols;
  int w1 = 0, w2 = 0, sum = 0, seenSum = 0;
  double m1, m2;
  double thresh1 = 0, thresh2 = 0, variance = 0, max = 0;
  for (int i = 1; i < 256; i++) {
    sum += hist[i] * i;
  }
  for (int i = 0; i < 256; i++) {
    w1 += hist[i];
    if (w1 == 0) continue;
    w2 = totalPixels - w1;
    if (w2 == 0) break;

    seenSum += i*hist[i];
    m1 = (double)seenSum / w1;
    m2 = (double)(sum - seenSum) / w2;
    variance = w1 * w2 * (m1 - m2) * (m1 - m2);
    if (variance >= max) {
      thresh1 = i;
      if (variance > max) {
        thresh2 = i;
      }
      max = variance;
    }
  }

  int cutoff = (thresh1 + thresh2) / 2.0;
  for (auto it = img.begin<uchar>(); it != img.end<uchar>(); it++) {
    *it = *it < cutoff ? BLACK : WHITE;
  }
  return cutoff;
}

void imGray(cv::Mat& img) {
  // Converts BGR image to grayscale using luminance
  cv::Mat gs(img.size(), CV_8U);
  for (int i = 0; i < img.rows; i++) {
    for (int j = 0; j < img.cols; j++) {
      cv::Vec3b bgr = img.at<cv::Vec3b>(i, j);
      gs.at<uchar>(i, j) = 0.07 * bgr[0] + 0.72 * bgr[1] + 0.21 * bgr[2];
    }
  }
  img = gs;
}


// Morphological operations, using a 3x3 square because it's easy

uchar dilateRegion(const cv::Mat& img, int i, int j, const std::vector<cv::Point2i>& region) {
  uchar max = 0;
  for (auto it = region.begin(); it != region.end(); it++) {
    int checkRow = i + it->y,
      checkCol = j + it->x;
    if (checkRow < img.rows && checkRow >= 0 && checkCol < img.cols && checkCol >= 0
        && img.at<uchar>(checkRow, checkCol) > max) {
      max = img.at<uchar>(checkRow, checkCol);
    }
  }
  return max;
}

cv::Mat imDilate(const cv::Mat& img, const std::vector<cv::Point2i>& shape) {
  cv::Mat dilation(img.size(), CV_8U);
  // Expects GS binary image
  for (int i = 0; i < img.rows; i++) {
    for (int j = 0; j < img.cols; j++) {
      // Dilation: if the pixel is white, mark all black pixels around it as GRAY,
      // and at the end shift the GRAY pixels to WHITE
      dilation.at<uchar>(i, j) = dilateRegion(img, i, j, shape);
    }
  }
  return dilation;
}

uchar erodeRegion(const cv::Mat& img, int i, int j, const std::vector<cv::Point2i>& region) {
  // Plus symbol
  uchar min = 255;
  for (auto it = region.begin(); it != region.end(); it++) {
    int checkRow = i + it->y,
      checkCol = j + it->x;
    if (checkRow < img.rows && checkRow >= 0 && checkCol < img.cols && checkCol >= 0
        && img.at<uchar>(checkRow, checkCol) < min) {
      min = img.at<uchar>(checkRow, checkCol);
    }
  }
  return min;
}

cv::Mat imErode(const cv::Mat& img, const std::vector<cv::Point2i>& shape) {
  cv::Mat erosion(img.size(), CV_8U);
  for (int i = 0; i < img.rows; i++) {
    for (int j = 0; j < img.cols; j++) {
      // Erosion: if the pixel is WHITE, search in the area of the shape. If a single BLACK pixel is detected,
      // make the center GRAY. At the end, sweep through and make the GRAY pixels BLACK
      erosion.at<uchar>(i, j) = erodeRegion(img, i, j, shape);
    }
  }
  return erosion;
}

cv::Mat imOpen(const cv::Mat& img, const std::vector<cv::Point2i>& shape) {
  return imDilate(imErode(img, shape), shape);
}

cv::Mat imClose(const cv::Mat& img, const std::vector<cv::Point2i>& shape) {
  return imErode(imDilate(img, shape), shape);
}

cv::Mat imFilterMask(const cv::Mat& img, const cv::Mat& mask) {
  cv::Mat filteredImage(img);
  for (int i = 0; i < img.rows; i++) {
    for (int j = 0; j < img.cols; j++) {
      if (mask.at<uchar>(i, j) == BLACK) {
        filteredImage.at<uchar>(i, j) = BLACK;
      }
    }
  }
  return filteredImage;
}
