#ifndef FREQFILT_H
#define FREQFILT_H

#include <iostream>
#include <vector>
#include <complex>
#include <math.h>
#include <assert.h>
#include <opencv2/opencv.hpp>

using cd = std::complex<double>;

const double PI = acos(-1);

// FFT and IFFT

void fft(std::vector<cd>&, bool);
void fft(std::vector<std::vector<cd>>&, bool);
void fftshift(std::vector<std::vector<cd>>&);

// filters

void ideal(std::vector<std::vector<cd>>&, double, int = 1, bool = false);
void gaussian(std::vector<std::vector<cd>>&, double, int = 1, bool = false);
void butterworth(std::vector<std::vector<cd>>&, double, int = 1, bool = false);

// helper functions

void transpose(std::vector<std::vector<cd>>&);
void conjugate(std::vector<std::vector<cd>>&);
void cofactor(std::vector<std::vector<cd>>&);
std::vector<std::vector<cd>> readVector(const cv::Mat&);
void writeVector(cv::Mat&, const std::vector<std::vector<cd>>&, bool = true);
cd dot(const cd&, const cd&);

#endif // FREQFILT_H
