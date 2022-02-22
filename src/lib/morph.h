#pragma once

// Morphological operations interface

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

#define X first
#define Y second

extern const int BINARY_THRESHOLD;
extern std::vector<std::string> OP_NAME;
extern std::vector<std::string> STRUCT_ELEM_NAME;
extern const std::vector<std::vector<std::pair<int, int>>> STEPS;

inline bool isValid(int x, int y, int rx, int ry) {
	return (x >= 0 && x < rx && y >= 0 && y < ry);
}

void apply(cv::Mat&, int, bool = false, int = 0);
void binary(cv::Mat&);
