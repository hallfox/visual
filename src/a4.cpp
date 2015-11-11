#include "visual.hpp"

#include <vector>

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
    *it = *it < cutoff ? 0 : 255;
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

void imDilate(cv::Mat& img) {

}

void imErode(cv::Mat& img) {
}
