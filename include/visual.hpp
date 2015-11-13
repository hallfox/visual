#ifndef _VISUAL_H
#define _VISUAL_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <queue>
#include <iostream>

#define PIXEL_RANGE 255

/****** ASSIGNMENT 1 FUNCTIONS ********/

int imSize(cv::Mat*);
void imNegative(cv::Mat*);
void imEqualize(cv::Mat*);
void imThresh(cv::Mat*, int);
int imAutoThresh(cv::Mat*);
void imRegionDetect(cv::Mat*);

/****** ASSIGNMENT 2 FUNCTIONS ********/

cv::Mat applyMask(const cv::Mat&, int*, int);
void imSharpen(cv::Mat&);
void imSobelEdge(cv::Mat&);
void imLogA(cv::Mat&);
void imLogB(cv::Mat&);

/****** ASSIGNMENT 3 FUNCTIONS ********/

float nalpha(int);
float freqComp(int, int, int, int);
void imToHSI(cv::Mat&);
cv::Mat imDct(const cv::Mat&, int);
cv::Mat imIdct(const cv::Mat&);
cv::Mat imLineDetect(cv::Mat&);
cv::Mat imCircDetect(cv::Mat&);
cv::Mat hsiToGs(const cv::Mat&);

/****** ASSIGNMENT 4 FUNCTIONS ********/

int imOtsuBinary(cv::Mat&);
void imGray(cv::Mat&);
cv::Mat imDilate(const cv::Mat&);
cv::Mat imErode(const cv::Mat&);
cv::Mat imOpen(const cv::Mat&);
cv::Mat imClose(const cv::Mat&);

#endif
