#pragma once

// Wrapper for handling BMP files

#include <stdlib.h>
#include <conio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <assert.h>

namespace bmp {
	// wrapper for BMP header
	#pragma pack(2)
	class Header {
	public:
		// file header
		uint16_t  type;                 // 2 bytes :'BM' for bitmap file
		uint32_t  size;                 // 4 bytes : File size in bytes
		uint16_t  reserved1;            // 2 bytes : Reserved space, not for editing
		uint16_t  reserved2;            // 2 bytes : Reserved space, not for editing
		uint32_t  offset;               // 4 bytes : Address at which pixel data starts in file

		// info header
		uint32_t  infoHeaderSize;       // 4 bytes : Size of File info header in bytes
		uint32_t  width;                // 4 bytes : Width of the image in pixels
		uint32_t  height;               // 4 bytes : Height of the image in pixels
		uint16_t  colorPlanes;          // 2 bytes : Number of color planes, fixed value 1 (not to be edited by user)
		uint16_t  bpp;                  // 2 bytes : Bits per pixel; for this program, 8bpp means 1 channel and 24bpp means 3 channel
		uint32_t  compression;          // 4 bytes : Compression method (not to be edited by user)
		uint32_t  imageSize;            // 4 bytes : the image size. This is the size of the raw bitmap data; a dummy 0 can be given for BI_RGB bitmaps (not to be edited by user)
		uint32_t  horizontalResolution; // 4 bytes : the horizontal resolution of the image. (pixel per meter, signed integer)
		uint32_t  verticalResolution;   // 4 bytes : the vertical resolution of the image. (pixel per meter, signed integer)
		uint32_t  numColor;             // 4 bytes : the number of colors in the color palette, or 0 to default to 2^n (not to be edited by user)
		uint32_t  imptColor;            // 4 bytes : the number of important colors used, or 0 when every color is important (not to be edited by user)

		/**
		* Streaming bmp::Header object via std::ostream object
		* Input:
		*	os:		std::ostream object
		*	header:	bmp::Header object
		*/
	};
	#pragma pack()

	class Image { public:
		//
		//  +-----------> (x) (width)
		//  |
		//  |   Axes defined in this way
		//  |
		// (y)
		// (height)
		//
		Header header;
		unsigned char*** pixels;

	public:
		Image() {}

		/**
		* Constructing bmp::Image object by reading from a .bmp file
		* Input:
		*	filename:	name of the image file (including .bmp extension)
		*/
		Image(const char *);

		/**
		* Deallocating resouces used up by bmp::Image object
		*/
		~Image();

		/**
		* Saving bmp::Image object to bmp file
		* Input:
		*	filename:	name of the bmp file (including .bmp extension)
		*/
		void save(const char *);

		/**
		* Converting a bmp::Image from RGB to GrayScale
		*/
		Image toGrayScale();

		/**
		* Constructs a diagonally-flipped version of bmp::Image object
		*/
		Image flip();

		/**
		* Constructs a rotated version of bmp::Image object
		* Input:
		*	degrees:	degrees to rotate (anti-clockwise)
		*/
		Image rotate(double);

		/**
		* Constructs a scaled version of bmp::Image object
		* Input:
		*	xscale:	scale along width
		*	yscale:	scale along height
		*/
		Image scale(double=1.0, double=1.0);

		/**
		* Constructs a new bmp::Image object by resetting one of the three channel
		* Input:
		*	chid:	'R' / 'G' / 'B'
		*/
		Image resetChannel(char);
	};

	/**
	* Reading a .bmp file into bmp::Image object and returning it
	* Input:
	*	filename:	name of the bmp file (including .bmp extension)
	* Output:
	*	a bmp::Image object
	*/
	Image read(const char *);

	/**
	* Writing bmp::Image object to a bmp file
	* Input:
	*	filename:	name of the bmp file (including .bmp extension)
	*	image:		bmp::Image object
	*/
	void write(const char *, Image&);
} // bmp 

std::ostream& operator<<(std::ostream&, const bmp::Header&);

