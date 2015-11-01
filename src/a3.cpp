#include "visual.hpp"

#include <cmath>

#define DCT_WINDOW_SIZE 8

void imToHSI(cv::Mat& image) {
  cv::Mat conv(image.size(), CV_32FC3);
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
      hsi[1] = 1 - rgbMin / hsi[2];

      // Normalize
      hsi[2] /= 255;

      conv.at<cv::Vec3f>(i, j) = hsi;
    }
  }
  image = conv;
}

/* Assumes HSI image
   // Let n = 8 (so u, v range from 0-7)

DCT transformation
r(x, y, u, v) = s(x, y, u, v) = a(u)a(v)cos((2x+1)u*pi/(2n)cos((2y+1)v*pi/2n)

a(u) = sqrt(1/n) if u = 0
       sqrt(2/n) if u != 0

 */

float nalpha(int u) {
  if (u == 0) {
    return sqrt(1.0 / DCT_WINDOW_SIZE);
  }
  else {
    return sqrt(2.0 / DCT_WINDOW_SIZE);
  }
}

float freqComp(int x, int y, int u, int v) {
  float uAlpha = nalpha(u);
  float vAlpha = nalpha(v);
  float xcos = cos((2.0*x+1)*u*M_PI / (2.0*DCT_WINDOW_SIZE));
  float ycos = cos((2.0*y+1)*v*M_PI / (2.0*DCT_WINDOW_SIZE));
  return uAlpha*vAlpha*xcos*ycos;
}

float dct(const cv::Mat& im, int i, int j, int u, int v) {
  float coeff = 0;
  for (int x = 0; x < DCT_WINDOW_SIZE; x++) {
    for (int y = 0; y < DCT_WINDOW_SIZE; y++) {
      coeff += im.at<float>(i+y, j+x)*freqComp(x, y, u, v);
    }
  }
  return coeff;
}

float idct(const cv::Mat& im, int i, int j, int x, int y) {
  float recov = 0;
  for (int u = 0; u < DCT_WINDOW_SIZE; u++) {
    for (int v = 0; v < DCT_WINDOW_SIZE; v++) {
      float coeff = im.at<float>(i+v, j+u);
      recov += coeff*freqComp(x, y, u, v);
    }
  }
  return recov;
}

void chanDct(cv::Mat& chan, int thresh) {
  for (int i = 0; i <= chan.rows - DCT_WINDOW_SIZE; i += DCT_WINDOW_SIZE) {
    for (int j = 0; j <= chan.cols - DCT_WINDOW_SIZE; j += DCT_WINDOW_SIZE) {
      // Find an 8x8 segment
      // Compute u, v using every x, y in 8x8 segment
      // Do 64 times (for u = 0:7, v = 0:7)
      // Find a new 8x8 segment
      // Repeat
      for (int u = 0; u < DCT_WINDOW_SIZE; u++) {
        for (int v = 0; v < DCT_WINDOW_SIZE; v++) {
          if (u < thresh && v < thresh) {
            chan.at<float>(i+v, j+u) = dct(chan, i, j, u, v);
          }
          else {
            chan.at<float>(i+v, j+u) = 0;
          }
        }
      }
    }
  }
}

void chanIdct(cv::Mat& chan) {
  for (int i = 0; i <= chan.rows - DCT_WINDOW_SIZE; i+=DCT_WINDOW_SIZE) {
    for (int j = 0; j <= chan.cols - DCT_WINDOW_SIZE; j+=DCT_WINDOW_SIZE) {
      // Find an 8x8 segment
      // Compute u, v using every x, y in 8x8 segment
      // Do 64 times (for u = 0:7, v = 0:7)
      // Find a new 8x8 segment
      // Repeat
      for (int x = 0; x < DCT_WINDOW_SIZE; x++) {
        for (int y = 0; y < DCT_WINDOW_SIZE; y++) {
          chan.at<float>(i+y, j+x) = idct(chan, i, j, x, y);
        }
      }
    }
  }
}

cv::Mat imDct(const cv::Mat& image, int thresh) {
  cv::Mat im = image.clone();
  cv::Mat fimage;
  cv::Mat chans[3];
  cv::split(im, chans);
  for (int i = 0; i < 3; i++) {
    chanDct(chans[i], thresh);
  }
  cv::merge(chans, 3, fimage);
  return fimage;
}

cv::Mat imIdct(const cv::Mat& image) {
  cv::Mat im = image.clone();
  cv::Mat fimage;
  cv::Mat chans[3];
  cv::split(im, chans);
  double min, max;
  for (int i = 0; i < 3; i++) {
    chanIdct(chans[i]);
    cv::minMaxLoc(chans[i], &min, &max);
    if (i == 0) {
      chans[i] = (chans[i] - min) * 359 / max;
    }
    else {
      chans[i] = (chans[i] - min) / max;
    }
  }
  cv::merge(chans, 3, fimage);
  return fimage;
}

cv::Mat imLineDetect(const cv::Mat& image) {
  cv::Mat im = image.clone();
  cv::Mat chans[3];
  imToHSI(im);
  cv::split(im, chans);
  cv::Mat gray = chans[2];
  imSobelEdge(gray);

  return gray;
}
