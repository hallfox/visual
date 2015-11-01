#include "visual.hpp"

#include <cmath>

#define DCT_WINDOW_SIZE 8

void imToHSI(cv::Mat& image) {
  cv::Mat hsi(image.size(), CV_32FC3);
  for (int i = 0; i < image.rows; i++) {
    for (int j = 0; j < image.cols; j++) {
      cv::Vec3b bgr = image.at<cv::Vec3b>(i, j);
      cv::Vec3f hsi;
      uchar red = bgr[2], green = bgr[1], blue = bgr[0];
      float theta = (180/M_PI) * acos(0.5*(2*red-green-blue) /
                         sqrt((red-green)*(red-green)+(red-blue)*(green-blue)));

      uchar rgbMin = 255;
      for (int k = 0; k < 3; k++) {
        rgbMin = bgr[k] < rgbMin ? bgr[k] : rgbMin;
      }

      hsi[0] = blue <= green ? theta : 360 - theta;
      hsi[2] = (1/3.0) * (red+green+blue);
      hsi[1] = 1 - rgbMin / hsi[1];
    }
  }
  image = hsi;
}

/* Assumes HSI image
   // Let n = 8 (so u, v range from 0-7)

DCT transformation
r(x, y, u, v) = s(x, y, u, v) = a(u)a(v)cos((2x+1)u*pi/(2n)cos((2y+1)v*pi/2n)

a(u) = sqrt(1/n) if u = 0
       sqrt(2/n) if u != 0

 */

inline float nalpha(int u) {
  if (u == 0) {
    return sqrt(1.0 / u);
  }
  else {
    return sqrt(2.0 / u);
  }
}

float freqComp(int x, int y, int u, int v) {
  float uAlpha = nalpha(u);
  float vAlpha = nalpha(v);
  return uAlpha * vAlpha * cos((2*x+1)*u*M_PI/(2*DCT_WINDOW_SIZE))
    * cos((2*y+1)*v*M_PI/(2*DCT_WINDOW_SIZE));
}

// cv::Mat imDct(const cv::Mat& image, int thresh) {
//   cv::Mat fimage(image.size(), CV_32FC3);
//   for (int i = 0; i < image.rows; i++) {
//     for (int j = 0; j < image.cols; j++) {
//       fimage = image.at<cv::Vec3f>(i, j)*freqComp(j, i, u, v);
//     }
//   }
// }
// cv::Mat imIdct(const cv::Mat& fimage) {
//   cv::Mat image;
//   for (int i = 0; i < image.rows; i++) {
//     for (int j = 0; j < image.cols; j++) {
//       image = fimage.at(i, j)*s;
//     }
//   }
// }
