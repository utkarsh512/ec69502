#define _CRT_SECURE_NO_WARNINGS

#include "morph.h"   // for morphological operations

// `opType` can be:
//	0 -> erosion
//	1 -> dilation
//	2 -> opening
//	3 -> closing
int opType = 0;

// `ElementType` can be:
//	0 -> Rectangular 1x2
//	1 -> Diamond 3x3
//	2 -> Square 3x3
//	3 -> Square 9x9
//	4 -> Square 15x15
int elementType = 0;

cv::Mat image;

/**
* performs the operations as per the selection specified by trackbars
*/
void onChange(int, void*) {
    std::cout << "[PENDING] " << OP_NAME[opType] << " operation using " << STRUCT_ELEM_NAME[elementType] << " element" << std::endl;

    // used recursive calls for higher order square elements
    int loop = 3 * (elementType - 2) * (elementType > 2);
    int type = elementType;
    if (type > 2) type = 2;

    // we need to perform erosion when opType = 0 or 2
    //                                 i.e. opType & 1 = 0 -> `flag` is false
    //
    // and dilation is performed when opType = 1 or 3
    //                                i.e. opType & 1 = 1 -> `flag` is true
    cv::Mat result = image.clone();
    apply(result, type, (opType & 1), loop);

    // in case of opening and closing (for both opType > 1), we need to
    // perform the reverse of operation we applied earlier
    if (opType > 1) {
        apply(result, type, !(opType & 1), loop);
    }

    cv::imshow("Morphed", result);
    std::string fname = "output_images\\" + OP_NAME[opType] + "\\" + STRUCT_ELEM_NAME[elementType] + ".bmp";
    cv::imwrite(fname, result);
    std::cout << "[DONE!]" << std::endl;
}

int main() {
    cv::namedWindow("Original");
    cv::namedWindow("Morphed");
    image = cv::imread("input_images\\ricegrains_mono.bmp", cv::IMREAD_GRAYSCALE);
    if (!image.data) {
        std::cout << "[Error]: Cannot open given file!" << std::endl;
        return 0;
    }
    binary(image);
    cv::createTrackbar("Operation", "Morphed", &opType, 3, onChange);
    cv::createTrackbar("Element", "Morphed", &elementType, 4, onChange);
    cv::imshow("Original", image);
    onChange(0, 0);
    cv::waitKey(0);
	return 0;
}
