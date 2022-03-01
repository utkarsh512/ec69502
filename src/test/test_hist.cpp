#include "hist.h"

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
