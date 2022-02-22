// Morphological operation implementation

#include "morph.h"

// constants

const int BINARY_THRESHOLD = 127;

std::vector<std::string> OP_NAME = { "erosion", "dilation", "opening", "closing" };

std::vector<std::string> STRUCT_ELEM_NAME = { "Rectangle 1x2", "Diamond 3x3", "Square 3x3", "Square 9x9", "Square 15x15" };

const std::vector<std::vector<std::pair<int, int>>> STEPS = {
	{{0, 0}, {0, 1}},
	{{0, 0}, {0, 1}, {0, -1}, {1, 0}, {-1, 0}},
	{{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 0}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}
};

/**
* applies structing element over `image` to perform given operation
* `type` should be 0 for Rectangle 1x2,
*                  1 for Diamond 3x3, and
*                  2 for Square 3x3
* `flag` should be true for dilation
*                  false for erosion
* `loop` is used for recursive calls when dimension of element is 9x9 or 15x15
*/
void apply(cv::Mat& image, int type, bool flag, int loop) {
	int intensity = 255 * flag;
	cv::Mat res = image.clone();
	for (int x = 0; x < image.rows; x++) {
		for (int y = 0; y < image.cols; y++) {
			if (image.at<uchar>(x, y) == intensity) {
				for (auto& step : STEPS[type])
					if (isValid(x + step.X, y + step.Y, image.rows, image.cols))
						res.at<uchar>(x + step.X, y + step.Y) = intensity;
			}
		}
	}
	image = res.clone();
	if (loop) {
		apply(image, type, flag, --loop);
	}
}

/**
* converting gray-scale image to binary
* thresholding done wrt BINARY_THRESHOLD
*/
void binary(cv::Mat& image) {
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			auto& pixel = image.at<uchar>(i, j);
			pixel = (pixel > BINARY_THRESHOLD) ? 255 : 0;
		}
	}
}
