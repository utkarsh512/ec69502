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
bmp::Image::Image(const char *filename) {
	FILE* f;
	fopen_s(&f, filename, "rb");
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
void bmp::Image::save(const char *filename) {
	FILE* f;
	fopen_s(&f, filename, "wb");
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
	bmp::Image image;
	image.pixels = new unsigned char** [1]; // grayscale bmp has only one channel
	image.pixels[0] = new unsigned char* [header.height];
	for (int i = 0; i < header.height; i++) {
		image.pixels[0][i] = new unsigned char[header.width];
	}
	for (int i = 0; i < header.height; i++) {
		for (int j = 0; j < header.width; j++) {
			image.pixels[0][i][j] = pixels[0][i][j] * 0.30 + pixels[1][i][j] * 0.59 + pixels[2][i][j] * 0.11;
		}
	}
	image.header = header;
	image.header.bpp = 8;
	image.header.offset = 1078;
	return image;
}

/**
* Constructs a diagonally-flipped version of bmp::Image object
*/
bmp::Image bmp::Image::flip() {
	bmp::Image image;
	int channel = header.bpp / 8;
	image.header = header;
	std::swap(image.header.width, image.header.height);
	image.pixels = new unsigned char** [channel];
	for (int k = 0; k < channel; k++) {
		image.pixels[k] = new unsigned char* [image.header.height];
		for (int i = 0; i < image.header.height; i++) {
			image.pixels[k][i] = new unsigned char[image.header.width];
			for (int j = 0; j < image.header.width; j++) {
				image.pixels[k][i][j] = pixels[k][header.height - j - 1][header.width - i - 1];
			}
		}
	}
	return image;
}

/**
* Constructs a rotated version of bmp::Image object
* Input:
*	degrees:	degrees to rotate (anti-clockwise)
*/
bmp::Image bmp::Image::rotate(double degrees) {
	bmp::Image image;

	// convert degrees into radians
	double radians = (degrees * 2 * acos(-1.0)) / 360;
	double sine = sin(radians);
	double cosine = cos(radians);

	// transforming the edge points
	std::pair<double, double> point1 = { header.height * sine * -1, header.height * cosine };
	std::pair<double, double> point2 = { header.width * cosine - header.height * sine, header.height * cosine + header.width * sine };
	std::pair<double, double> point3 = { header.width * cosine, header.width * sine };

	double minX = std::min(0.0, std::min(point1.first, std::min(point2.first, point3.first)));
	double minY = std::min(0.0, std::min(point1.second, std::min(point2.second, point3.second)));
	double maxX = std::max(point1.first, std::max(point2.first, point3.first));
	double maxY = std::max(point1.second, std::max(point2.second, point3.second));

	image.header = header;
	uint32_t newWidth = static_cast<uint32_t>(ceil(fabs(maxX) - minX));
	uint32_t newHeight = static_cast<uint32_t>(ceil(fabs(maxY) - minY));

	int channel = header.bpp / 8;
	image.pixels = new unsigned char** [channel];
	for (int i = 0; i < channel; i++) {
		image.pixels[i] = new unsigned char* [header.height];
		for (int j = 0; j < header.height; j++) {
			image.pixels[i][j] = new unsigned char[header.width];
			for (int k = 0; k < header.width; k++) {
				image.pixels[i][j][k] = 0;
			}
		}
	}

	// filling pixels value
	for (int k = 0; k < channel; k++) {
		for (int x = 0; x < header.width; x++) {
			for (int y = 0; y < header.height; y++) {
				double xx = 1.0 * x * newWidth / header.width;
				double yy = 1.0 * y * newHeight / header.height;
				int srcX = static_cast<int>((xx + minX) * cosine + (yy + minY) * sine);
				int srcY = static_cast<int>((yy + minY) * cosine - (xx + minX) * sine);
				if (srcX >= 0 && srcX < header.width && srcY >= 0 && srcY < header.height) {
					image.pixels[k][y][x] = pixels[k][srcY][srcX];
				}
			}
		}
	}
	return image;
}

/**
* Constructs a scaled version of bmp::Image object
* Input:
*	xscale:	scale along width
*	yscale:	scale along height
*/
bmp::Image bmp::Image::scale(double xscale, double yscale) {
	bmp::Image image;
	image.header = header;
	int channel = header.bpp / 8;

	// calculating new height, widht and size
	image.header.width = static_cast<uint32_t>(ceil(xscale * header.width));
	image.header.height = static_cast<uint32_t>(ceil(yscale * header.height));
	image.header.imageSize = image.header.width * image.header.height * channel;
	image.header.size += image.header.imageSize - header.imageSize;

	image.pixels = new unsigned char** [channel];
	for (int k = 0; k < channel; k++) {
		image.pixels[k] = new unsigned char* [image.header.height];
		for (int i = 0; i < image.header.height; i++) {
			image.pixels[k][i] = new unsigned char[image.header.width];
			for (int j = 0; j < image.header.width; j++) {
				image.pixels[k][i][j] = 0;
			}
		}
	}

	// filling pixel values
	for (int k = 0; k < channel; k++) {
		for (int i = 0; i < header.height; i++) {
			for (int j = 0; j < header.width; j++) {
				int p = ceil(yscale * i);
				int q = ceil(xscale * j);
				image.pixels[k][p][q] = pixels[k][i][j];
			}
		}
	}
	return image;
}

/**
* Constructs a new bmp::Image object by resetting one of the three channel
* Input:
*	chid:	'R' / 'G' / 'B'
*/
bmp::Image bmp::Image::resetChannel(char chid) {
	bmp::Image image;
	int channel = header.bpp / 8;
	image.header = header;
	int ch;
	if (chid == 'R') ch = 0;
	else if (chid == 'G') ch = 1;
	else if (chid == 'B') ch = 2;
	else throw std::runtime_error("Bad chid passed for resetChannel!");

	image.pixels = new unsigned char** [channel];
	for (int k = 0; k < channel; k++) {
		image.pixels[k] = new unsigned char* [header.height];
		for (int i = 0; i < header.height; i++) {
			image.pixels[k][i] = new unsigned char[header.width];
			for (int j = 0; j < header.width; j++) {
				image.pixels[k][i][j] = (k != ch) ? pixels[k][i][j] : 0;
			}
		}
	}
	return image;
}

/**
* Reading from bmp file to bmp::Image object and returning it
* Input:
*	filename:	name of the bmp file (including .bmp extension)
* Output:
*	a bmp::Image object
*/
bmp::Image bmp::read(const char *filename) {
	return bmp::Image(filename);
}

/**
* Writing bmp::Image object to a bmp file
* Input:
*	filename:	name of the bmp file (including .bmp extension)
*	image:		bmp::Image object
*/
void bmp::write(const char *filename, bmp::Image& image) {
	image.save(filename);
}
