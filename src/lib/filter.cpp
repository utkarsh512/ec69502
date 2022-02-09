#include "filt.h"

// default constructor for Kernel
Kernel::Kernel() : size(0) {}

// parametrized constructor for Kernel
Kernel::Kernel(int filterType, int filterSize) : size(filterSize * 2 + 3) {
	matrix = FILTERS[filterType][filterSize];
}

// Get value at any given position in Kernel
double Kernel::at(int i, int j) {
	i += size / 2;
	j += size / 2;
	return matrix[size * i + j];
}

// performs convolution with cv::Mat object
cv::Mat Kernel::conv(cv::Mat& image) {
	cv::Mat result = image.clone();
	int size2 = size / 2;

	for (int i = 0; i < result.rows; i++) {
		for (int j = 0; j < result.cols; j++) {
			double pixel = 0;
			for (int dx = -size2; dx <= size2; dx++) {
				for (int dy = -size2; dy <= size2; dy++) {
					int nx = i + dx;
					int ny = j + dy;
					if (nx >= 0 && nx < result.rows && ny >= 0 && ny < result.cols) {
						pixel += at(dx, dy) * static_cast<int>(image.at<uchar>(nx, ny));
					}
				}
			}
			if (pixel > 255) pixel = 255;
			else if (pixel < 0) pixel = 0;
			result.at<uchar>(i, j) = static_cast<uchar>(pixel);
		}
	}
	return result;
}

// performs averaging
cv::Mat mean(cv::Mat& image, int size) {
	cv::Mat result = image.clone();
	int size2 = size / 2;

	for (int i = 0; i < result.rows; i++) {
		for (int j = 0; j < result.cols; j++) {
			int sum = 0, count = 0;
			for (int dx = -size2; dx <= size2; dx++) {
				for (int dy = -size2; dy <= size2; dy++) {
					int nx = i + dx;
					int ny = j + dy;
					if (nx >= 0 && nx < result.rows && ny >= 0 && ny < result.cols) {
						sum += static_cast<int>(image.at<uchar>(nx, ny));
					}
					count++;
				}
			}
			result.at<uchar>(i, j) = static_cast<uchar>(sum / count);
		}
	}
	return result;
}

// performs median filtering
cv::Mat median(cv::Mat& image, int size) {
	cv::Mat result = image.clone();
	int size2 = size / 2;

	for (int i = 0; i < result.rows; i++) {
		for (int j = 0; j < result.cols; j++) {
			std::vector<uchar> pixels;
			for (int dx = -size2; dx <= size2; dx++) {
				for (int dy = -size2; dy <= size2; dy++) {
					int nx = i + dx;
					int ny = j + dy;
					if (nx >= 0 && nx < result.rows && ny >= 0 && ny < result.cols) {
						pixels.push_back(image.at<uchar>(nx, ny));
					}
				}
			}
			std::sort(pixels.begin(), pixels.end());
			result.at<uchar>(i, j) = pixels[size2];
		}
	}
	return result;
}
