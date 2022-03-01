#include "freqfilt.h"

int main() {
	cv::Mat image = cv::imread("input_images\\dip.tif", cv::IMREAD_GRAYSCALE);
	auto matrix = readVector(image); // reading image in 2-D vector
	cofactor(matrix);                // applying (-1)^(i + j) operation
	fft(matrix, false);              // computing fft
	conjugate(matrix);               // computing complex conjugate
	fft(matrix, true);               // computing ifft
	cofactor(matrix);                // applying (-1)^(i + j) operation
	cv::Mat result = image.clone();
	writeVector(result, matrix, false);     
	cv::namedWindow("Input image");
	cv::imshow("Input image", image);
	cv::namedWindow("Output image");
	cv::imshow("Output image", result);
	cv::waitKey(0);
}
