#include "visual.hpp"

/** All functions expect a GS image **/

uchar maskAvg(const cv::Mat& img, int x, int y, int w) {
  int avg = 0;
  for (int i = y; i < y+w && i < img.rows; i++) {
    for (int j = x; j < x+w && j < img.cols; j++) {
      avg += img.at<uchar>(i, j);
    }
  }
  return static_cast<uchar>(avg / (w*w));
}

void maskFill(cv::Mat& img, int x, int y, int w, uchar fill_val) {
  for (int i = y; i < y+w && i < img.rows; i++) {
    for (int j = x; j < x+w && j < img.cols; j++) {
      img.at<uchar>(i, j) = fill_val;
    }
  }
}

double euclidDistance(const cv::Mat& img1, const cv::Mat& img2, cv::Point2i p1, cv::Point2i p2, int w) {
  double dist = 0;
  for (int x = 0; x < w; x++) {
    for (int y = 0; y < w; y++) {
      cv::Point2i loc1(p1.x+x, p1.y+y), loc2(p2.x+x, p2.y+y);
      dist += std::pow(img1.at<uchar>(loc1) - img2.at<uchar>(loc2), 2);
    }
  }
  return std::sqrt(dist);
}

cv::Point2i findNNEuclid(const cv::Mat& img, cv::Point2i p) {
  cv::Point2i nearest(0, 0);
  double minDist = std::numeric_limits<double>::max();
  for (int i = 0; i < img.rows/2; i+=4) {
    for (int j = 0; j < img.cols; j+=4) {
      double dist = euclidDistance(img, img, p, cv::Point2i(j, i), 4);
      if (dist < minDist) {
        minDist = dist;
        nearest = cv::Point2i(j, i);
      }
    }
  }
  return nearest;
}

cv::Point2i findNN(const cv::Mat& avgs, uchar target) {
  cv::Point2i nearest(0, 0);

  uchar val;
  int min_thresh = 256;

  for (int i = 0; i < avgs.rows; i++) {
    for (int j = 0; j < avgs.cols; j++) {
      val = avgs.at<uchar>(i, j);
      if (std::abs(val - target) < min_thresh) {
        min_thresh = std::abs(val - target);
        nearest = cv::Point2i(j, i);
      }
    }
  }
  return nearest;
}

cv::Mat imNNClassify(const cv::Mat& src) {
  int rows = src.rows, cols = src.cols;
  cv::Mat dst(rows / 4, cols / 4, CV_8U);

  for (int i = 0; i < rows / 2; i+=4) {
    for (int j = 0; j < cols; j+=4) {
      dst.at<uchar>(i/4, j/4) = maskAvg(src, j, i, 4);
    }
  }
  return dst;
}

void imManualTrain(const cv::Mat& src, cv::Mat& dst) {
  int rows = src.rows, cols = src.cols;
  cv::Mat res = src.clone();
  uchar avg;

  for (int i = 0; i < rows/2; i+=4) {
    for (int j = 0; j < cols; j+=4) {
      avg = maskAvg(src, j, i, 4);
      uchar val;
      if (0 <= avg && avg < 125) {
        val = 0;
      }
      else if (125 <= avg && avg < 175) {
        val = 128;
      }
      else {
        val = 255;
      }
      maskFill(res, j, i, 4, val);
    }
  }
  dst = res;
}

void imManualTest(const cv::Mat& src, cv::Mat& dst) {
  int rows = src.rows, cols = src.cols;
  cv::Mat res = src.clone();
  uchar avg;

  for (int i = rows/2; i < rows; i+=4) {
    for (int j = 0; j < cols; j+=4) {
      avg = maskAvg(src, j, i, 4);
      uchar val;
      if (0 <= avg && avg < 125) {
        val = 0;
      }
      else if (125 <= avg && avg < 175) {
        val = 128;
      }
      else {
        val = 255;
      }
      maskFill(res, j, i, 4, val);
    }
  }
  dst = res;
}

void imNNTrain(const cv::Mat& src, cv::Mat& dst) {
  cv::Mat res = src.clone();

  int rows = src.rows, cols = src.cols;
  for (int i = rows/2; i < rows; i+=4) {
    for (int j = 0; j < cols; j+=4) {
      cv::Point2i nearest = findNNEuclid(src, cv::Point2i(j, i));
      uchar avg = maskAvg(src, nearest.x, nearest.y, 4);
      if (0 <= avg && avg < 125) {
        maskFill(res, j, i, 4, 0);
      }
      else if (125 <= avg && avg < 175) {
        maskFill(res, j, i, 4, 128);
      }
      else {
        maskFill(res, j, i, 4, 255);
      }
    }
  }

  dst = res;
}

void imNNCopy(const cv::Mat& src, cv::Mat& dst) {
  cv::Mat res = src.clone();
  cv::Mat training_data = imNNClassify(src);
  uchar val;
  cv::Point2i nearest;

  int rows = src.rows, cols = src.cols;
  for (int i = rows / 2; i < rows; i+=4) {
    for (int j = 0; j < cols; j+=4) {
      val = maskAvg(src, j, i, 4);
      nearest = findNN(training_data, val);
      int r = nearest.y*4, c = nearest.x*4;
      for (int p = 0; p < 4; p++) {
        for(int q = 0; q < 4; q++) {
          res.at<uchar>(i+p, j+q) = src.at<uchar>(r+p, c+q);
        }
      }
    }
  }
  dst = res;
}

void imNNAverage(const cv::Mat& src, cv::Mat& dst) {
  cv::Mat res = src.clone();
  cv::Mat training_data = imNNClassify(src);
  uchar val;
  cv::Point2i nearest;

  int rows = src.rows, cols = src.cols;
  for (int i = rows / 2; i < rows; i+=4) {
    for (int j = 0; j < cols; j+=4) {
      val = maskAvg(src, j, i, 4);
      nearest = findNN(training_data, val);
      maskFill(res, j, i, 4, training_data.at<uchar>(nearest));
    }
  }
  dst = res;
}
