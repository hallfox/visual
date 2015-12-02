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

double imError(const cv::Mat& img1, const cv::Mat& img2) {
  double err = 0;
  for (int i = 0; i < img1.rows; i++) {
    for (int j = 0; j < img2.rows; j++) {
      err += std::pow(img1.at<uchar>(i, j) - img2.at<uchar>(i, j), 2);
    }
  }
  return err / (img1.rows * img1.cols);
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

void calcClassAvg(const cv::Mat & img, uchar *min, uchar *mid, uchar *max) {
  long c1 = 0, c2 = 0, c3 = 0;
  int count1 = 0, count2 = 0, count3 = 0;
  for (int i = 0; i < img.rows/2; i+=4) {
    for (int j = 0; j < img.cols; j+=4) {
      uchar avg = maskAvg(img, j, i, 4);
      if (0 <= avg && avg < 125) {
        c1 += avg*16;
        count1++;
      }
      else if (125 <= avg && avg < 175) {
        c2 += avg*16;
        count2++;
      }
      else {
        c3 += avg*16;
        count3++;
      }
    }
  }
  *min = static_cast<uchar>(c1 / (count1*16));
  *mid = static_cast<uchar>(c2 / (count2*16));
  *max = static_cast<uchar>(c3 / (count3*16));
}

void imNNClassAverage(const cv::Mat& src, cv::Mat& dst) {
  cv::Mat res = src.clone();
  uchar min, max, mid;
  calcClassAvg(src, &min, &mid, &max);

  int rows = src.rows, cols = src.cols;
  for (int i = rows/2; i < rows; i+=4) {
    for (int j = 0; j < cols; j+=4) {
      cv::Point2i nearest = findNNEuclid(src, cv::Point2i(j, i));
      uchar avg = maskAvg(src, nearest.x, nearest.y, 4);
      if (0 <= avg && avg < 125) {
        maskFill(res, j, i, 4, min);
      }
      else if (125 <= avg && avg < 175) {
        maskFill(res, j, i, 4, mid);
      }
      else {
        maskFill(res, j, i, 4, max);
      }
    }
  }

  dst = res;
}

std::vector<uchar> computeCenter(const cv::Mat& img, const std::vector<cv::Point2i>& group) {
  std::vector<long> center(16, 0);
  std::vector<uchar> res(16);
  for (auto it = group.begin(); it != group.end(); it++) {
    for (int x = 0; x < 4; x++) {
      for (int y = 0; y < 4; y++) {
        if (it->x+x < img.cols && it->y+y < img.rows) {
          // std::cout << cv::Point2i(it->x+x, it->y+y) << "\n";
          center[x+y*4] += img.at<uchar>(cv::Point2i(it->x+x, it->y+y));
        }
      }
    }
  }
  for (int i = 0; i < center.size(); i++) {
    res[i] = static_cast<uchar>(center[i] / group.size());
  }
  return res;
}

double euclidDistance(const cv::Mat& img, cv::Point2i p, const std::vector<uchar>& v, int w) {
  double dist = 0;
  for (int x = 0; x < w; x++) {
    for (int y = 0; y < w; y++) {
      cv::Point2i loc(p.x+x, p.y+y);
      if (loc.x < img.cols && loc.y < img.rows) {
        // std::cout << "Current loc: " << loc << std::endl;
        dist += std::pow(img.at<uchar>(loc) - v[x+y*4], 2);
      }
    }
  }
  return std::sqrt(dist);
}

void imKMeans(const cv::Mat& src, cv::Mat& dst) {
  cv::Mat res = src.clone();
  std::vector<uchar> centers[3];
  std::vector<cv::Point2i> groups[3] = {
    {cv::Point2i(0, 0)},
    {cv::Point2i(4, 0)},
    {cv::Point2i(8, 0)},
  };
  for (int i = 0; i < 3; i++) {
    centers[i] = computeCenter(src, groups[i]);
    // std::cout << "Initial groups: " << groups[i] << "\n";
  }
  for (int i = 0; i < src.rows/2; i+=4) {
    for (int j = 0; j < src.cols; j+=4) {
      if (i == 0 && j <= 8) {
        continue;
      }

      cv::Point2i loc(j, i);
      // std::cout << loc << "\n";
      double minDist = std::numeric_limits<double>::max();
      int nearest = 0;
      for (int k = 0; k < 3; k++) {
        double dist = euclidDistance(src, loc, centers[k], 4);
        if (dist < minDist) {
          minDist = dist;
          nearest = k;
        }
      }
      groups[nearest].push_back(loc);
      // std::cout << "Updated group " << nearest << " " << groups[nearest] << std::endl;
      centers[nearest] = computeCenter(src, groups[nearest]);
      switch(nearest) {
      case 0:
        maskFill(res, j, i, 4, 0);
        break;
      case 1:
        maskFill(res, j, i, 4, 128);
        break;
      case 2:
        maskFill(res, j, i, 4, 255);
        break;
      }
    }
  }
  dst = res;
}

/** Part 2 **/
void imDiff(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& dst) {
  cv::Mat diff(src1.size(), CV_8U);
  for (int i = 0; i < src1.rows; i++) {
    for (int j = 0; j < src1.cols; j++) {
      diff.at<uchar>(i, j) = std::abs(src1.at<uchar>(i, j) - src2.at<uchar>(i, j));
    }
  }
  dst = diff;
}

cv::Mat diffTest(std::string file1, std::string file2) {
  cv::Mat img1 = cv::imread(file1);
  cv::Mat img2 = cv::imread(file2);
  cv::Mat diff;
  imGray(img1);
  imGray(img2);
  imDiff(img1, img2, diff);
  return diff;
}

cv::Point2i nearestDiff(const cv::Mat& img1, const cv::Mat& img2, cv::Point2i p) {
  cv::Point2i nearest(0, 0);
  cv::Mat diff(img1.size(), CV_8U);
  double minDist = std::numeric_limits<double>::max();
  for (int i = 0; i < img2.rows; i+=8) {
    for (int j = 0; j < img2.cols; j+=8) {
      double dist = euclidDistance(img1, img2, p, cv::Point2i(j, i), 8);
      if (dist < minDist) {
        minDist = dist;
        nearest = cv::Point2i(j, i);
      }
    }
  }
  return nearest;
}

void compDiff(const cv::Mat& img1, const cv::Mat& img2, cv::Mat& dst) {
  cv::Mat diff(img1.size(), CV_8U);
  for (int i = 0; i < img1.rows; i+=8) {
    for (int j = 0; j < img1.cols; j+=8) {
      cv::Point2i nearest = nearestDiff(img1, img2, cv::Point2i(j, i));
      for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
          diff.at<uchar>(i+y, j+x) = std::abs(img1.at<uchar>(i+y, j+x) - img2.at<uchar>(nearest.y+y, nearest.x+x));
        }
      }
    }
  }
  dst = diff;
}

cv::Mat compDiffTest(std::string file1, std::string file2) {
  cv::Mat img1 = cv::imread(file1);
  cv::Mat img2 = cv::imread(file2);
  cv::Mat diff;
  imGray(img1);
  imGray(img2);
  compDiff(img1, img2, diff);
  return diff;
}
