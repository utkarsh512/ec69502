#define _CRT_SECURE_NO_WARNINGS
#include "freqfilt.h"  // for fft, filters
#include <Windows.h>   // for extracting filenames

int fileName = 0;                   // index of current image in `fileList`
int filterType = 0;                 // index of current filter 
int threshold = 0;                  // index to current value of D
int order = 0;                      // index to current scale (to D)
std::vector<std::string> fileList;  // vector of filename of the images

std::vector<std::string> FILTER_NAME = {
    "idealLPF", "idealHPF", "gaussianLPF",
    "gaussianHPF", "butterworthLPF", "butterworthHPF"
};

// fetch all the images from given directory
void getFiles(const std::string& directory, std::vector<std::string>& out) {
    HANDLE dir;
    WIN32_FIND_DATA fileData;
    wchar_t dirL[256];
    mbstowcs((wchar_t*)dirL, (directory + "/*.jpg").c_str(), 256);
    if ((dir = FindFirstFile(dirL, &fileData)) == INVALID_HANDLE_VALUE)
        return; /* No files found */

    do {
        char fileNameRaw[256];
        wcstombs((char*)fileNameRaw, fileData.cFileName, 256);
        const std::string fileName = fileNameRaw;
        const std::string fullFileName = directory + "/" + fileName;
        const bool isDirectory = (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

        if (fileName[0] == '.')
            continue;

        if (isDirectory)
            continue;

        out.push_back(fullFileName);
    } while (FindNextFile(dir, &fileData));

    FindClose(dir);
}

// called when something is changed in the slider
void onChange(int, void*) {
    // reading the image
    cv::Mat image = cv::imread(fileList[fileName], cv::IMREAD_GRAYSCALE);
    if (!image.data) {
        std::cout << "[Error]: Cannot open given file!" << std::endl;
        return;
    }

    cv::imshow("[S] Input", image);
    auto matrix = readVector(image);               // reading values into 2-D vector `matrix`
    fft(matrix, false);                            // performing FFT over the image
    fftshift(matrix);
    cv::Mat inputSpectrum = image.clone();
    writeVector(inputSpectrum, matrix); // computing magnitude spectrum
    cv::imshow("[F] Input", inputSpectrum);

    // selecting filter
    int n = matrix.size();
    std::vector<std::vector<cd>> filter(n, std::vector<cd>(n));
    int filterType_ = (filterType >> 1);
    bool high = filterType % 2;
    int threshold_ = (1 << (threshold << 1)) - 1;

    if (filterType_ == 0) {
        ideal(filter, threshold_, order + 1, high);
    }
    else if (filterType_ == 1) {
        gaussian(filter, threshold_, order + 1, high);
    }
    else {
        butterworth(filter, threshold_, order + 1, high);
    }
    cv::Mat filterSpectrum = image.clone();
    writeVector(filterSpectrum, filter); // magnitude spectrum of filter
    cv::imshow("[F] Filter", filterSpectrum);

    // applying filter
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) {
        matrix[i][j] = dot(matrix[i][j], filter[i][j]);
    }
    cv::Mat outputSpectrum = image.clone();
    writeVector(outputSpectrum, matrix); // magnitude spectrum of output
    cv::imshow("[F] Output", outputSpectrum);

    // performing IFFT to get filtered output image
    fft(matrix, true);
    cv::Mat outputImage = image.clone();
    writeVector(outputImage, matrix, false);
    cv::imshow("[S] Output", outputImage);

    std::string message = "[Done!] filter=" + FILTER_NAME[filterType] + ", ";
    message += "threshold=" + std::to_string(threshold_) + ", ";
    message += "order=" + std::to_string(order);
    std::cout << message << std::endl;
}

int main() {
    getFiles("input_images", fileList);
    cv::namedWindow("[S] Input");
    cv::namedWindow("[S] Output");
    cv::namedWindow("[F] Input");
    cv::namedWindow("[F] Output");
    cv::namedWindow("[F] Filter");
    cv::createTrackbar("Source", "[S] Input", &fileName, fileList.size() - 1, onChange);
    cv::createTrackbar("Filter", "[S] Output", &filterType, 5, onChange);
    cv::createTrackbar("Threshold", "[S] Output", &threshold, 5, onChange);
    cv::createTrackbar("Order", "[S] Output", &order, 3, onChange);
    onChange(0, 0);
    cv::waitKey(0);
	return 0;
}
