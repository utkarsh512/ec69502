
// Module to perform histogram equalization and matching

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <conio.h>
#include <math.h>

#include <opencv2/opencv.hpp>

typedef std::vector< std::vector<double>> Vec;

const int LEVEL = 256;
const int INTENSITY_MAX = 255;
const int INTENSITY_MIN = 0;

/**
* Function to generate histogram from the single-channel
* Input:
*	image:	cv::Mat object
* Output:
*	histogram:	std::vector<double> object
*/
std::vector<double> imageToHistogram(const cv::Mat& image) {
	std::vector<double> histogram(LEVEL);
	int tot = 0;
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			int idx = static_cast<int>(image.at<uchar>(i, j));
			histogram[idx]++;
			tot++;
		}
	}
	for (auto& x : histogram) x /= tot;
	return histogram;
}

/**
* Function to generate histogram from the multi-channel
* Input:
*	image:	cv::Mat object
* Output:
*	histogram:	std::vector<std::vector<double>> object
*/
Vec imageToHistogram(const cv::Mat& image, int channels) {
	Vec histograms(channels, std::vector<double>(LEVEL));
	int tot = 0;
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			cv::Vec3b pixel = image.at<cv::Vec3b>(i, j);
			for (int k = 0; k < 3; k++) {
				histograms[k][static_cast<int>(pixel[k])]++;
			}
			tot++;
		}
	}
	for (auto& histogram : histograms) {
		for (auto& val : histogram) {
			val /= tot;
		}
	}
	return histograms;
}

/**
* Function to generate transfer function for given histogram for single-channel
* (In this case, it is cumulative density function)
* Input:
*	histogram:	std::vector<double> object
* Output:
*	transfer:	std::vector<double> object
*/
std::vector<double> getTransferFunction(const std::vector<double>& histogram) {
	std::vector<double> transfer(histogram);
	for (int i = 1; i < transfer.size(); i++) {
		transfer[i] += transfer[i - 1];
	}
	return transfer;
}

/**
* Function to generate transfer function for given histogram for multi-channel
* (In this case, it is cumulative density function)
* Input:
*	histogram:	std::vector<std::vector<double>> object
* Output:
*	transfer:	std::vector<std::vector<double>> object
*/
Vec getTransferFunction(const Vec& histograms) {
	Vec transfers;
	for (auto& histogram : histograms) {
		transfers.push_back(getTransferFunction(histogram));
	}
	return transfers;
}

/**
* Function to perform histogram equalization for single-channel
* Input:
*	transfer:	std::vector<double> object
* Output:
*	histogram:	std::vector<double> object
*/
std::vector<double> histogramEqualization(const std::vector<double>& transfer) {
	std::vector<double> histogram(LEVEL);
	double transferMin = transfer[0];
	for (int i = 0; i < LEVEL; i++) {
		histogram[i] = (transfer[i] - transferMin) / (1 - transferMin);
		histogram[i] *= LEVEL - 1;
		histogram[i] += 0.5;
	}
	return histogram;
}

/**
* Function to perform histogram equalization for multi-channel
* Input:
*	transfer:	std::vector<std::vector<double>> object
* Output:
*	histogram:	std::vector<std::vector<double>> object
*/
Vec histogramEqualization(const Vec& transfers) {
	Vec histograms;
	for (int i = 0; i < transfers.size(); i++) {
		histograms.push_back(histogramEqualization(transfers[i]));
	}
	return histograms;
}

/**
* Function to perform histogram matching for single-channel
* Input:
*	inputTransfer:	std::vector<double> object
*	targetTransfer:	std::vector<double> object
* Output:
*	histogram:	std::vector<double> object
*/
std::vector<double> histogramMatching(const std::vector<double>& inputTransfer, const std::vector<double>& targetTransfer) {
	std::vector<double> histogram(LEVEL);
	for (int i = 0; i < LEVEL; i++) {
		int j = 0;
		do {
			histogram[i] = j++;
		} while (j < LEVEL && inputTransfer[i] > targetTransfer[j]);
	}
	return histogram;
}

/**
* Function to perform histogram matching for single-channel
* Input:
*	inputTransfer:	std::vector<std::vector<double>> object
*	targetTransfer:	std::vector<std::vector<double>> object
* Output:
*	histogram:	std::vector<std::vector<double>> object
*/
Vec histogramMatching(const Vec& inputTransfers, const Vec& targetTransfers) {
	Vec histograms;
	for (int i = 0; i < inputTransfers.size(); i++) {
		histograms.push_back(histogramMatching(inputTransfers[i], targetTransfers[i]));
	}
	return histograms;
}

/**
* Function to display histogram using OpenCV libraries for single-channel
*/
void showHistogram(cv::Mat& image, const std::string& filename) {
	cv::Mat histogram = cv::Mat::zeros(1, LEVEL, CV_32SC1);
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			uchar intensity = image.at<uchar>(i, j);
			histogram.at<int>(intensity) += 1;
		}
	}
	int hmax = 0;
	for (int i = 0; i < LEVEL; i++) {
		hmax = std::max(hmax, histogram.at<int>(i));
	}
	cv::Mat canvas = cv::Mat::ones(125, LEVEL, CV_8UC3);
	for (int j = 0, rows = canvas.rows; j < LEVEL; j++) {
		line(canvas, cv::Point(j, rows),
			cv::Point(j, rows - (histogram.at<int>(j) * rows / hmax)),
			cv::Scalar(255, 255, 255), 1, 8, 0);
	}
	cv::imshow(filename, canvas);
}

/**
* Function to display histogram using OpenCV libraries for multi-channel
*/
void showHistogram(cv::Mat& image, int channel, const std::string& filename) {
	cv::Mat histogram = cv::Mat::zeros(1, LEVEL, CV_32SC1);
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			cv::Vec3b pixel = image.at<cv::Vec3b>(i, j);
			histogram.at<int>(pixel[channel]) += 1;
		}
	}
	int hmax = 0;
	for (int i = 0; i < LEVEL; i++) {
		hmax = std::max(hmax, histogram.at<int>(i));
	}
	cv::Mat canvas = cv::Mat::ones(125, LEVEL, CV_8UC3);
	for (int j = 0, rows = canvas.rows; j < LEVEL; j++) {
		line(canvas, cv::Point(j, rows),
			cv::Point(j, rows - (histogram.at<int>(j) * rows / hmax)),
			cv::Scalar(255, 255, 255), 1, 8, 0);
	}
	cv::imshow(filename, canvas);
}

/**
* Function to interactivily read an image using OpenCV
*/
cv::Mat readImage(std::string& filename, const std::string& type, int FLAG=0) {
	std::cout << "Enter " << type << " " << " filename: ";
	std::cin >> filename;
	if (FLAG == 0) {
		filename = "C:\\Users\\Utkarsh\\Documents\\acads\\assignments\\image-proc\\img\\equalization\\" + filename;
	}
	else if (FLAG == 1) {
		filename = "C:\\Users\\Utkarsh\\Documents\\acads\\assignments\\image-proc\\img\\matching\\source\\" + filename;
	}
	else {
		filename = "C:\\Users\\Utkarsh\\Documents\\acads\\assignments\\image-proc\\img\\matching\\target\\" + filename;
	}
	cv::Mat image = cv::imread(filename);
	return image;
}

/**
* Function to perform histogram equalization of single-channel images
*/
void EqualizationSingle(cv::Mat& image) {
	auto histogram = imageToHistogram(image);
	auto transfer = getTransferFunction(histogram);
	auto outputHistogram = histogramEqualization(transfer);

	cv::Mat outputImage = image.clone();
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			outputImage.at<uchar>(i, j) = cv::saturate_cast<uchar>(cv::saturate_cast<int>(outputHistogram[image.at<uchar>(i, j)]));
		}
	}

	cv::namedWindow("Original image");
	cv::imshow("Original image", image);
	showHistogram(image, "Original Histogram");

	cv::namedWindow("Histogram-equalized image");
	cv::imshow("Histogram-equalized image", outputImage);
	showHistogram(outputImage, "Equalized histogram");

	cv::waitKey();
}

/**
* Function to perform histogram equalization of multi-channel images
*/
void EqualizationMulti(cv::Mat& image) {
	auto histograms = imageToHistogram(image, 3);
	auto transfers = getTransferFunction(histograms);
	auto outputHistograms = histogramEqualization(transfers);

	cv::Mat outputImage = image.clone();
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			cv::Vec3b& intensity = outputImage.at<cv::Vec3b>(i, j);
			for (int k = 0; k < image.channels(); k++) {
				int val = static_cast<int>(intensity.val[k]);
				intensity.val[k] = cv::saturate_cast<uchar>(cv::saturate_cast<int>(outputHistograms[k][val]));
			}
		}
	}

	cv::namedWindow("Original image");
	cv::imshow("Original image", image);
	showHistogram(image, 0, "[0] Original Histogram");
	showHistogram(image, 1, "[1] Original Histogram");
	showHistogram(image, 2, "[2] Original Histogram");

	cv::namedWindow("Histogram-equalized image");
	cv::imshow("Histogram-equalized image", outputImage);
	showHistogram(outputImage, 0, "[0] Equalized histogram");
	showHistogram(outputImage, 1, "[1] Equalized histogram");
	showHistogram(outputImage, 2, "[2] Equalized histogram");

	cv::waitKey();
}

/**
* Function to perform histogram matching of single-channel image wrt single-channel image
*/
void MatchingOneToOne(cv::Mat& source, cv::Mat& target) {
	auto sourceHistogram = imageToHistogram(source);
	auto targetHistogram = imageToHistogram(target);
	auto sourceTransfer = getTransferFunction(sourceHistogram);
	auto targetTransfer = getTransferFunction(targetHistogram);
	auto outputHistogram = histogramMatching(sourceTransfer, targetTransfer);

	cv::Mat outputImage = source.clone();
	for (int i = 0; i < source.rows; i++) {
		for (int j = 0; j < source.cols; j++) {
			outputImage.at<uchar>(i, j) = cv::saturate_cast<uchar>(cv::saturate_cast<int>(outputHistogram[source.at<uchar>(i, j)]));
		}
	}

	cv::namedWindow("Source image");
	cv::imshow("Source image", source);
	showHistogram(source, "Source histogram");

	cv::namedWindow("Target image");
	cv::imshow("Target image", target);
	showHistogram(target, "Target histogram");

	cv::namedWindow("Histogram-matched image");
	cv::imshow("Histogram-matched image", outputImage);
	showHistogram(outputImage, "Equalized histogram");

	cv::waitKey();
}

/**
* Function to perform histogram matching of multi-channel image wrt multi-channel image
*/
void MatchingManyToMany(cv::Mat& source, cv::Mat& target) {
	auto sourceHistograms = imageToHistogram(source, 3);
	auto targetHistograms = imageToHistogram(target, 3);
	auto sourceTransfers = getTransferFunction(sourceHistograms);
	auto targetTransfers = getTransferFunction(targetHistograms);
	auto outputHistograms = histogramMatching(sourceTransfers, targetTransfers);

	cv::Mat outputImage = source.clone();
	for (int i = 0; i < source.rows; i++) {
		for (int j = 0; j < source.cols; j++) {
			cv::Vec3b& intensity = outputImage.at<cv::Vec3b>(i, j);
			for (int k = 0; k < source.channels(); k++) {
				int val = static_cast<int>(intensity.val[k]);
				intensity.val[k] = cv::saturate_cast<uchar>(cv::saturate_cast<int>(outputHistograms[k][val]));
			}
		}
	}

	cv::namedWindow("Source image");
	cv::imshow("Source image", source);
	showHistogram(source, 0, "[0] Source histogram");
	showHistogram(source, 1, "[1] Source histogram");
	showHistogram(source, 2, "[2] Source histogram");

	cv::namedWindow("Target image");
	cv::imshow("Target image", target);
	showHistogram(target, 0, "[0] Target histogram");
	showHistogram(target, 1, "[1] Target histogram");
	showHistogram(target, 2, "[2] Target histogram");

	cv::namedWindow("Histogram-matched image");
	cv::imshow("Histogram-matched image", outputImage);
	showHistogram(outputImage, 0, "[0] Equalized histogram");
	showHistogram(outputImage, 1, "[1] Equalized histogram");
	showHistogram(outputImage, 2, "[2] Equalized histogram");

	cv::waitKey();
}

/**
* Function to perform histogram matching of multi-channel image wrt single-channel image
*/
void MatchingManyToOne(cv::Mat& source, cv::Mat& target) {
	auto sourceHistograms = imageToHistogram(source, 3);
	auto targetHistogram = imageToHistogram(target);
	auto sourceTransfers = getTransferFunction(sourceHistograms);
	auto targetTransfer = getTransferFunction(targetHistogram);
	Vec targetTransfers(3, targetTransfer);
	auto outputHistograms = histogramMatching(sourceTransfers, targetTransfers);

	cv::Mat outputImage = source.clone();
	for (int i = 0; i < source.rows; i++) {
		for (int j = 0; j < source.cols; j++) {
			cv::Vec3b& intensity = outputImage.at<cv::Vec3b>(i, j);
			for (int k = 0; k < source.channels(); k++) {
				int val = static_cast<int>(intensity.val[k]);
				intensity.val[k] = cv::saturate_cast<uchar>(cv::saturate_cast<int>(outputHistograms[k][val]));
			}
		}
	}

	cv::namedWindow("Source image");
	cv::imshow("Source image", source);
	showHistogram(source, 0, "[0] Source histogram");
	showHistogram(source, 1, "[1] Source histogram");
	showHistogram(source, 2, "[2] Source histogram");

	cv::namedWindow("Target image");
	cv::imshow("Target image", target);
	showHistogram(target, "Target histogram");

	cv::namedWindow("Histogram-matched image");
	cv::imshow("Histogram-matched image", outputImage);
	showHistogram(outputImage, 0, "[0] Equalized histogram");
	showHistogram(outputImage, 1, "[1] Equalized histogram");
	showHistogram(outputImage, 2, "[2] Equalized histogram");

	cv::waitKey();
}

int main() {
	int option;
	std::string filename1, filename2;

	while (1) {
		std::cout << "+++++++++++++++++++++++++++++++++++++++++\n";
		std::cout << "Enter the opcode:\n";
		std::cout << "\t1. Histogram equalization\n";
		std::cout << "\t2. Histogram matching\n";
		std::cout << "Select (1 or 2): ";
		std::cin >> option;
		std::cout << "+++++++++++++++++++++++++++++++++++++++++\n";
		if (option == 1) {
			// histogram equalization
			std::cout << "Histogram equalization selected\n";
			cv::Mat image = readImage(filename1, "input");
			if (image.empty()) {
				throw std::runtime_error("Error reading the file!");
			}
			if (image.channels() == 1) {
				EqualizationSingle(image);
			}
			else if (image.channels() == 3) {
				EqualizationMulti(image);
			}
			else {
				throw std::runtime_error("Incompatible number of channels!");
			}
		}
		else if (option == 2) {
			// histogram matching selected
			std::cout << "Histogram matching selected\n";
			cv::Mat source = readImage(filename1, "source", 1);
			cv::Mat target = readImage(filename2, "target", 2);
			if (source.empty() || target.empty()) {
				throw std::runtime_error("Error reading the files!");
			}
			int sourceChannels = source.channels();
			int targetChannels = target.channels();
			if (sourceChannels == 1 && targetChannels == 1) {
				MatchingOneToOne(source, target);
			}
			else if (sourceChannels == 3 && targetChannels == 3) {
				MatchingManyToMany(source, target);
			}
			else if (sourceChannels == 3 && targetChannels == 1) {
				MatchingManyToOne(source, target);
			}
			else {
				throw std::runtime_error("Incompatible number of channels in source and target files!");
			}
		}
		else {
			throw std::runtime_error("Wrong opcode received!");
		}
	}
	return 0;
}
