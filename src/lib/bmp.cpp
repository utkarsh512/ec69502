#include "bmp.h"

/**
* Streaming bmp::Header object via std::ostream object
* Input:
*	os: std::ostream object
*	h:	bmp::Header object
*/
std::ostream& operator<<(std::ostream& os, const bmp::Header& h) {
	os << "(bmp::Header at " << &h << ")\n";
	os << "(type: " << static_cast<char>(h.type) << static_cast<char>(h.type >> 8) << ")\n";
	os << "(size: " << h.size << ")\n";
	os << "(offset: " << h.offset << ")\n";
	os << "(info h size: " << h.infoHeaderSize << ")\n";
	os << "(bitmap width: " << h.width << ")\n";
	os << "(bitmap height " << h.height << ")\n";
	os << "(color planes: " << h.colorPlanes << ")\n";
	os << "(BPP: " << h.bpp << ")\n";
	os << "(compression method " << h.compression << ")\n";
	os << "(raw bmp data size: " << h.imageSize << ")\n";
	os << "(horizontal resolution: " << h.horizontalResolution << ")\n";
	os << "(vertical resolution: " << h.verticalResolution << ")\n";
	os << "(no. of color: " << h.numColor << ")\n";
	os << "(no. of imp colors: " << h.imptColor << ")\n";
	return os;
}

/**
* Constructing bmp::Image object by reading from a .bmp file
* Input:
*	filename:	name of the image file (including .bmp extension)
*/
bmp::Image::Image(const std::string& filename) {
	FILE* f;
	fopen_s(&f, filename.c_str(), "rb");
	if (!f) {
		throw std::runtime_error("File doesn\'t exists!");
	}

	// reading the header
	fread(&header, sizeof(unsigned char), sizeof(bmp::Header), f);
	std::cout << header;

	// number of channels
	int channel = header.bpp / 8;

	int iter = (header.offset - sizeof(bmp::Header)) / sizeof(unsigned char);
	int tmp;
	for (int i = 0; i < iter; i++) {
		fread(&tmp, sizeof(unsigned char), sizeof(unsigned char), f);
	}

	// reading pixel data
	pixels = new unsigned char** [channel];
	for (int i = 0; i < channel; i++) {
		pixels[i] = new unsigned char* [header.height];
		for (int j = 0; j < header.height; j++) {
			pixels[i][j] = new unsigned char[header.width];
		}
	}
	int paddingSize = header.width * channel;
	while (paddingSize % 4) paddingSize++;

	// in bmp, pixel data is stored bottom-to-top, left-to-right
	for (int i = header.height - 1; i >= 0; i--) {
		fseek(f, header.offset + paddingSize * i, SEEK_SET);
		for (int j = 0; j < header.width; j++) {
			for (int k = channel - 1; k >= 0; k--) {
				fread(&(pixels[k][i][j]), sizeof(unsigned char), sizeof(unsigned char), f);
			}
		}
	}

	fclose(f);
}

/**
* Deallocating resouces used up by bmp::Image object
*/
bmp::Image::~Image() {
	for (int i = 0; i < header.bpp / 8; i++) {
		for (int j = 0; j < header.height; j++) {
			delete[] pixels[i][j];
		}
		delete[] pixels[i];
	}
	delete[] pixels;
}

/**
* Saving bmp::Image object to bmp file
* Input:
*	filename:	name of the bmp file (including .bmp extension)
*/
void bmp::Image::save(const std::string& filename) {
	FILE* f;
	fopen_s(&f, filename.c_str(), "wb");
	if (!f) {
		throw std::runtime_error("Cannot create this file!");
	}

	// writing header to the file
	fwrite(&header, sizeof(unsigned char), sizeof(bmp::Header), f);
	int channel = header.bpp / 8;

	// writing extra data to the file
	//	Writing extra data (that lies between end of headr and offset) to the file
	int k = 0;
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 3; j++)
			fwrite(&i, sizeof(unsigned char), sizeof(unsigned char), f);
		fwrite(&k, sizeof(unsigned char), sizeof(unsigned char), f);
	}

	// writing pixel data
	int paddingSize = header.width * channel;
	while (paddingSize % 4) paddingSize++;
	for (int h = header.height - 1; h >= 0; h--) {
		fseek(f, header.offset + paddingSize * h, SEEK_SET);
		for (int w = 0; w < header.width; w++) {
			for (int k = channel - 1; k >= 0; k--) {
				fwrite(&(pixels[k][h][w]), sizeof(unsigned char), sizeof(unsigned char), f);
			}
		}
	}

	fclose(f);
}

/**
* Converting a bmp::Image from RGB to GrayScale
*/
bmp::Image bmp::Image::toGrayScale() {
	int channel = header.bpp / 8;

	// asserts that given bmp::Image has RGB channels
	assert(channel == 3);

	// grayscale bmp::Image object
	bmp::Image gray;
	gray.pixels = new unsigned char** [1]; // grayscale bmp has only one channel
	gray.pixels[0] = new unsigned char* [header.height];
	for (int i = 0; i < header.height; i++) {
		gray.pixels[0][i] = new unsigned char[header.width];
	}
	for (int i = 0; i < header.height; i++) {
		for (int j = 0; j < header.width; j++) {
			gray.pixels[0][i][j] = pixels[0][i][j] * 0.30 + pixels[1][i][j] * 0.59 + pixels[2][i][j] * 0.11;
		}
	}
	gray.header = header;
	gray.header.bpp = 8;
	gray.header.offset = 1078;
	return gray;
}

/**
* Reading from bmp file to bmp::Image object and returning it
* Input:
*	filename:	name of the bmp file (including .bmp extension)
* Output:
*	a bmp::Image object
*/
bmp::Image bmp::read(const std::string& filename) {
	return bmp::Image(filename);
}

/**
* Writing bmp::Image object to a bmp file
* Input:
*	filename:	name of the bmp file (including .bmp extension)
*	image:		bmp::Image object
*/
void bmp::write(const std::string& filename, bmp::Image& image) {
	image.save(filename);
}
