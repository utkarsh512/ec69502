#ifndef HIST_H
#define HIST_H

#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>

typedef std::vector< std::vector<double>> Vec;

const int LEVEL = 256;
const int INTENSITY_MAX = 255;
const int INTENSITY_MIN = 0;

/**
* Function to generate histogram from the single-channel
*/
std::vector<double> imageToHistogram(const cv::Mat&);

/**
* Function to generate histogram from the multi-channel
*/
Vec imageToHistogram(const cv::Mat&, int);

/**
* Function to generate transfer function for given histogram for single-channel
* (In this case, it is cumulative density function)
*/
std::vector<double> getTransferFunction(const std::vector<double>&);

/**
* Function to generate transfer function for given histogram for multi-channel
* (In this case, it is cumulative density function)
*/
Vec getTransferFunction(const Vec&);

/**
* Function to perform histogram equalization for single-channel
*/
std::vector<double> histogramEqualization(const std::vector<double>&);

/**
* Function to perform histogram equalization for multi-channel
*/
Vec histogramEqualization(const Vec&);

/**
* Function to perform histogram matching for single-channel
*/
std::vector<double> histogramMatching(const std::vector<double>&, const std::vector<double>&);

/**
* Function to perform histogram matching for single-channel
*/
Vec histogramMatching(const Vec&, const Vec&);

/**
* Function to display histogram using OpenCV libraries for single-channel
*/
void showHistogram(cv::Mat&, const std::string&);

/**
* Function to display histogram using OpenCV libraries for multi-channel
*/
void showHistogram(cv::Mat&, int, const std::string&);

/**
* Function to interactivily read an image using OpenCV
*/
cv::Mat readImage(std::string&, const std::string&, int=0);

/**
* Function to perform histogram equalization of single-channel images
*/
void EqualizationSingle(cv::Mat&);

/**
* Function to perform histogram equalization of multi-channel images
*/
void EqualizationMulti(cv::Mat&);

/**
* Function to perform histogram matching of single-channel image wrt single-channel image
*/
void MatchingOneToOne(cv::Mat&, cv::Mat&);

/**
* Function to perform histogram matching of multi-channel image wrt multi-channel image
*/
void MatchingManyToMany(cv::Mat&, cv::Mat&);

/**
* Function to perform histogram matching of multi-channel image wrt single-channel image
*/
void MatchingManyToOne(cv::Mat&, cv::Mat&);

#endif // HIST_H