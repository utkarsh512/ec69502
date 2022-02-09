#include "filt.h"

int main() {
    cv::Mat image = cv::imread(
        "C:\\Users\\Utkarsh\\Documents\\acads\\assignments\\image-proc\\img\\filtering\\normal\\lena_gray_512.jpg",
        cv::IMREAD_GRAYSCALE
    );
    
    double b = 0.4;
    double s = 70;

    cv::Mat result = adaptiveHighBoost(image, b, s);
    std::string outfile = "C:\\Users\\Utkarsh\\Documents\\acads\\assignments\\image-proc\\img\\filtering\\results\\AHB_";
    outfile += std::to_string(b) + "_" + std::to_string(s) + ".jpg";

    cv::imwrite(outfile, result);
    cv::namedWindow("Input image");
    cv::imshow("Input image", image);
    cv::namedWindow("Output image");
    cv::imshow("Output image", result);
    cv::waitKey(0);
}
