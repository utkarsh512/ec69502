#include "filt.h"

int fileName = 0;
int filterType = 0;
int filterSize = 0;
std::vector<std::string> fileList;

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

// perform filtering when filter type is changed via slider
void onFilterChange(int, void*) {
    std::cout << "Applying " << FILTER_NAME[filterType] << " filter" << std::endl;
    cv::Mat output = cv::imread(fileList[fileName], cv::IMREAD_GRAYSCALE);
    if (filterType == 0) {}
    else if (filterType == 2) {
        output = median(output, filterSize * 2 + 3);
    }
    else {
        Kernel kernel(filterType, filterSize);
        output = kernel.conv(output);
    }
    cv::imshow("Output Image", output);
    std::string outputFilename = "C:\\Users\\Utkarsh\\Documents\\acads\\assignments\\image-proc\\img\\filtering\\results\\";
    outputFilename += std::to_string(fileName) + "_" + std::to_string(filterType) + "_" + std::to_string(filterSize) + ".jpg";
    cv::imwrite(outputFilename, output);
    std::cout << "Done!" << std::endl;
}

// performs filtering when filter size os changed via slider
void onSizeChange(int, void*) {
    std::cout << "Size of filter changed to " << 2 * filterSize + 3 << std::endl;
    onFilterChange(filterType, 0);
}


// performs filtering when input image is changed via slider
void onInputChange(int, void*) {
    std::cout << "Opening: " << fileList[fileName] << std::endl;
    cv::Mat img = cv::imread(fileList[fileName], cv::IMREAD_GRAYSCALE);

    if (!img.data) {
        std::cout << "Error:Image not found" << std::endl;
        return;
    }
    cv::imshow("Input Image", img);
    onFilterChange(filterType, 0);
}

// automated filtering of all the files with every possible filter
void automatic() {
    std::vector<int> validFileNames, validFilterTypes, validFilterSize;
    for (int i = 0; i < fileList.size(); i++) validFileNames.push_back(i);
    for (int i = 0; i < 11; i++) validFilterTypes.push_back(i);
    for (int i = 0; i < 4; i++) validFilterSize.push_back(i);
    for (int x : validFileNames) {
        fileName = x;
        for (int y : validFilterTypes) {
            filterType = y;
            for (int z : validFilterSize) {
                filterSize = z;
                onSizeChange(0, 0);
            }
        }
    }
}

int main() {
    getFiles("C:\\Users\\Utkarsh\\Documents\\acads\\assignments\\image-proc\\img\\filtering\\noisy", fileList);
    getFiles("C:\\Users\\Utkarsh\\Documents\\acads\\assignments\\image-proc\\img\\filtering\\normal", fileList);

    cv::namedWindow("Input Image");
    cv::namedWindow("Output Image");

    std::string currentFile = fileList[1];

    cv::createTrackbar("Source", "Input Image", &fileName, fileList.size() - 1, onInputChange);
    cv::createTrackbar("Filter", "Output Image", &filterType, 10, onFilterChange);
    cv::createTrackbar("Size", "Output Image", &filterSize, 3, onSizeChange);

    onInputChange(0, 0);
    onFilterChange(0, 0);
    // automatic();
    cv::waitKey(0);
}
