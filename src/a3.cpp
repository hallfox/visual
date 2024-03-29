#include "visual.hpp"

#include <opencv2/imgproc.hpp>

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
      hsi[2] /= 256;
      hsi[0] *= 1./360;

      conv.at<cv::Vec3f>(i, j) = hsi;
    }
  }
  image = conv;
}

void imFromHSI(cv::Mat& image) {
  cv::Mat conv(image.size(), CV_8UC3);
  for (int i = 0; i < image.rows; i++) {
    for (int j = 0; j < image.cols; j++) {
      cv::Vec3f hsi = image.at<cv::Vec3f>(i, j);
      cv::Vec3b bgr;

      float hue = hsi[0] * 360;
      float red, green, blue;
      if (hue >= 0 && hue < 120) {
        blue = hsi[2] * (1 - hsi[1]);
        red = hsi[2] * (1 + (hsi[1]*cos((M_PI/180)*hue) / cos((M_PI/180)*(60 - hue))));
        green = 3*hsi[2] - (red + blue);
      }
      else if (hue >= 120 && hue < 240) {
        hue -= 120;
        red = hsi[2] * (1 - hsi[1]);
        green = hsi[2] * (1 + (hsi[1]*cos((M_PI/180)*hue) / cos((M_PI/180)*(60 - hue))));
        blue = 3*hsi[2] - (red + green);
      }
      else {
        hue -= 240;
        green = hsi[2] * (1 - hsi[1]);
        blue = hsi[2] * (1 + (hsi[1]*cos((M_PI/180)*hue) / cos((M_PI/180)*(60 - hue))));
        red = 3*hsi[2] - (blue + green);
      }

      bgr = {static_cast<uchar>(blue * 256), static_cast<uchar>(green * 256), static_cast<uchar>(red * 256)};

      conv.at<cv::Vec3b>(i, j) = bgr;
    }
  }
  image = conv;
}

cv::Mat hsiToGs(const cv::Mat& img) {
  cv::Mat chans[3];
  cv::split(img, chans);
  chans[2].convertTo(chans[2], CV_8U, 255);
  return chans[2];
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
  cv::Mat dctMat(chan.size(), CV_32F);
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
            dctMat.at<float>(i+v, j+u) = dct(chan, i, j, u, v);
          }
          else {
            dctMat.at<float>(i+v, j+u) = 0;
          }
        }
      }
    }
  }
  chan = dctMat;
}

void chanIdct(cv::Mat& chan) {
  cv::Mat idctMat(chan.size(), CV_32F);
  for (int i = 0; i <= chan.rows - DCT_WINDOW_SIZE; i+=DCT_WINDOW_SIZE) {
    for (int j = 0; j <= chan.cols - DCT_WINDOW_SIZE; j+=DCT_WINDOW_SIZE) {
      // Find an 8x8 segment
      // Compute u, v using every x, y in 8x8 segment
      // Do 64 times (for u = 0:7, v = 0:7)
      // Find a new 8x8 segment
      // Repeat
      for (int x = 0; x < DCT_WINDOW_SIZE; x++) {
        for (int y = 0; y < DCT_WINDOW_SIZE; y++) {
          idctMat.at<float>(i+y, j+x) = idct(chan, i, j, x, y);
        }
      }
    }
  }
  chan = idctMat;
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
  for (int i = 0; i < 3; i++) {
    chanIdct(chans[i]);
  }
  cv::merge(chans, 3, fimage);
  return fimage;
}

inline int linRadius(int x, int y, int theta) {
  return x*cos(M_PI * theta / 180) + y*sin(M_PI * theta / 180);
}

cv::Mat linHough(const cv::Mat& image) {
  int d = sqrt(image.cols*image.cols + image.rows*image.rows) + 1;
  cv::Mat votes = cv::Mat::zeros(90, d, CV_8U);

  for (int i = 0; i < image.rows; i++) {
    for (int j = 0; j < image.cols; j++) {
      // Do voting
      if (image.at<uchar>(i, j) > 10) {
        // x = j, y = i
        for (int theta = 0; theta < 90; theta++) {
          int r = linRadius(j, i, theta);
          if (r < d) votes.at<uchar>(theta, r)++;
        }
      }
    }
  }
  std::cout << votes;
  return votes;
}

cv::Mat imLineDetect(cv::Mat& image) {
  cv::Mat im = image.clone();
  cv::Mat chans[3];
  imToHSI(im);
  cv::split(im, chans);
  cv::Mat gray;
  chans[2].convertTo(gray, CV_8U, 255);
  imSobelEdge(gray);
  cv::Mat votes = linHough(gray);
  for (int i = 0; i < 30; i++) {
    cv::Point maxVotes;
    cv::minMaxLoc(votes, 0, 0, 0, &maxVotes);
    int r = maxVotes.x;
    double t = M_PI * maxVotes.y / 180;
    int x1 = 0, x2 = image.cols;
    int y1 = (r - x1*cos(t)) / sin(t), y2 = (r - x2*cos(t)) / sin(t);
    cv::line(image, cv::Point(x1, y1), cv::Point(x2, y2), CV_RGB(0, 255, 0));
    votes.at<uchar>(maxVotes) = 0;
  }
  return votes;
}

void circVote(cv::Mat& votes, int x0, int y0, int r, int xdim, int ydim) {
  // Midpoint circle algorithm, blatantly ripped off from Wikipedia
  // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
  // I hope this works because if it doesn't god save us all
  // At least it was a good idea
  int x = r;
  int y = 0;
  int d2 = 1 - x;
  std::vector<cv::Point> visit;

  while (y <= x) {
    visit = {
      cv::Point(x+x0, y+y0),
      cv::Point(y+x0, x+y0),
      cv::Point(-x+x0, y+y0),
      cv::Point(-y+x0, x+y0),
      cv::Point(-x+x0, -y+y0),
      cv::Point(-y+x0, -x+y0),
      cv::Point(x+x0, -y+y0),
      cv::Point(x+x0, -y+y0)
    };
    for (auto it = visit.begin(); it != visit.end(); it++) {
      if (it->x < xdim && it->x >= 0 && it->y < ydim && it->y >= 0) {
        votes.at<unsigned short>(it->x, it->y, r)++;
      }
    }
    y++;
    if (d2 <= 0) {
      d2 += 2 * y + 1;
    }
    else {
      x--;
      d2 += 2* (y-x) + 1;
    }
  }
}

cv::Mat circHough(const cv::Mat& image) {
  // r**2 = (x - x0)**2 + (y - y0)**2
  int sz[3] = {image.cols, image.rows, image.rows/2};
  cv::Mat votes = cv::Mat::zeros(3, sz, CV_16U);

  for (int r = 0; r < image.rows/2; r++) {
    for (int i = 0; i < image.rows; i++) {
      for (int j = 0; j < image.rows/2; j++) {
        // Do voting
        if (image.at<uchar>(i, j) > 10) {
          // x = j, y = i
          circVote(votes, j, i, r, image.cols, image.rows);
        }
      }
    }
  }
  return votes;
}

cv::Mat imCircDetect(cv::Mat& image) {
  cv::Mat im = image.clone();
  imToHSI(im);
  cv::Mat gray = hsiToGs(im);
  imSobelEdge(gray);
  cv::Mat votes = circHough(gray);
  int max = 0;
  cv::Point3i maxPoint;
  for (int x = 0; x < image.cols; x++) {
    for (int y = 0; y < image.rows; y++) {
      for (int r = 0; r < image.rows/2; r++) {
        if (votes.at<unsigned short>(x, y, r) > max) {
          max = votes.at<unsigned short>(x, y, r);
          maxPoint = cv::Point3i(x, y, r);
        }
      }
    }
  }
  std::cout << "Max is: " << max << " at " << maxPoint << "\n";
  cv::circle(image, cv::Point(maxPoint.x, maxPoint.y), maxPoint.z, CV_RGB(0, 255, 0));
  return votes;
}
