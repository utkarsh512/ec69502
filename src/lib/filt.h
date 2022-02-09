#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <algorithm>
#include <vector>
#include <opencv2/opencv.hpp>
#include <Windows.h>

using Vec2d = std::vector<std::vector<double>>;
using Vec3d = std::vector<Vec2d>;

// filter declaration

// high pass
extern std::vector<double> HIGH_PASS;
// /high pass

// mean
extern std::vector<double> MEAN_3;
extern std::vector<double> MEAN_5;
extern std::vector<double> MEAN_7;
extern std::vector<double> MEAN_9;
// /mean

// prewitt
extern std::vector<double> PREWITT_H_3;
extern std::vector<double> PREWITT_V_3;
extern std::vector<double> PREWITT_H_5;
extern std::vector<double> PREWITT_V_5;
extern std::vector<double> PREWITT_H_7;
extern std::vector<double> PREWITT_V_7;
extern std::vector<double> PREWITT_H_9;
extern std::vector<double> PREWITT_V_9;
// /prewitt

// sobel
extern std::vector<double> SOBEL_H_3;
extern std::vector<double> SOBEL_V_3;
extern std::vector<double> SOBEL_D_3;
extern std::vector<double> SOBEL_H_5;
extern std::vector<double> SOBEL_V_5;
extern std::vector<double> SOBEL_D_5;
extern std::vector<double> SOBEL_H_7;
extern std::vector<double> SOBEL_V_7;
extern std::vector<double> SOBEL_D_7;
extern std::vector<double> SOBEL_H_9;
extern std::vector<double> SOBEL_V_9;
extern std::vector<double> SOBEL_D_9;
// /sobel

// laplacian
extern std::vector<double> LAPLACIAN_3;
extern std::vector<double> LAPLACIAN_5;
extern std::vector<double> LAPLACIAN_7;
extern std::vector<double> LAPLACIAN_9;
// /laplacian

// gaussian
extern std::vector<double> GAUSSIAN_3;
extern std::vector<double> GAUSSIAN_5;
extern std::vector<double> GAUSSIAN_7;
extern std::vector<double> GAUSSIAN_9;
// /gaussian

// LAPLACIAN_OF_GAUSSIAN
extern std::vector<double> LAPLACIAN_OF_GAUSSIAN_3;
extern std::vector<double> LAPLACIAN_OF_GAUSSIAN_5;
extern std::vector<double> LAPLACIAN_OF_GAUSSIAN_7;
extern std::vector<double> LAPLACIAN_OF_GAUSSIAN_9;
// /LAPLACIAN_OF_GAUSSIAN

extern const char* FILTER_NAME[11];

extern Vec2d MEAN;
extern Vec2d PREWITT_H;
extern Vec2d PREWITT_V;
extern Vec2d SOBEL_H;
extern Vec2d SOBEL_V;
extern Vec2d SOBEL_D;
extern Vec2d LAPLACIAN;
extern Vec2d GAUSSIAN;
extern Vec2d LAPLACIAN_OF_GAUSSIAN;
extern Vec3d FILTERS;

// filter declaration ends

// creating different kernels
class Kernel { 
	int size;
	std::vector<double> matrix;

public:
	Kernel();
	Kernel(int, std::vector<double>&);
	Kernel(int, int);
	double at(int, int);
	cv::Mat conv(cv::Mat&);
};

cv::Mat median(cv::Mat&, int);                       // performs median filtering
cv::Mat adaptiveHighBoost(cv::Mat&, double, double); // performs adaptive HB filtering
