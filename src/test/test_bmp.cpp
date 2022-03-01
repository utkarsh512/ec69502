// reading, writing and handling bmp files defined in this header
#include "bmp.h"

// path to the referenced bmp files
const char *lena          = "img/lena_colored_256.bmp";
const char *camera        = "img/cameraman.bmp";
const char *corn          = "img/corn.bmp";

// path to bmp files that will be generated through this program
const char *lenaGrayScale = "img/lena_gray_scale.bmp";
const char *lenaFlipped   = "img/lena_flipped.bmp";
const char *cameraFlipped = "img/camera_flipped.bmp";
const char *lenaRot45     = "img/lena_rotate_45_degrees.bmp";
const char *lenaRot90     = "img/lena_rotate_90_degrees.bmp";
const char *cameraRot45   = "img/camera_rotate_45_degrees.bmp";
const char *cameraRot90   = "img/camera_rotate_90_degrees.bmp";
const char *lenaScaled    = "img/lena_scaled.bmp";
const char *cameraScaled  = "img/camera_scaled.bmp";
const char *cornR         = "img/corn_without_r.bmp";
const char *cornG         = "img/corn_without_g.bmp";
const char *cornB         = "img/corn_without_b.bmp";

// parameters for scaling
const double xscale       = 2.0;
const double yscale       = 1.0;

int main() {
	// reading referenced bmp files
	bmp::Image iLena(lena), iCamera(camera), iCorn(corn);

	// creating a grayscaled verison of lena
	bmp::Image iLenaGrayScale = iLena.toGrayScale();
	iLenaGrayScale.save(lenaGrayScale);

	// diagonally flipping lena
	bmp::Image iLenaFlipped = iLena.flip();
	iLenaFlipped.save(lenaFlipped);

	// diagonally flipping cameraman
	bmp::Image iCameraFlipped = iCamera.flip();
	iCameraFlipped.save(cameraFlipped);

	// 45 degrees rotation to lena
	bmp::Image iLenaRot45 = iLena.rotate(45);
	iLenaRot45.save(lenaRot45);

	// 90 degrees rotation to lena
	bmp::Image iLenaRot90 = iLena.rotate(90);
	iLenaRot90.save(lenaRot90);

	// 45 degrees rotation to cameraman
	bmp::Image iCameraRot45 = iCamera.rotate(45);
	iCameraRot45.save(cameraRot45);

	// 90 degrees rotation to cameraman
	bmp::Image iCameraRot90 = iCamera.rotate(90);
	iCameraRot90.save(cameraRot90);

	// scaling lena
	bmp::Image iLenaScaled = iLena.scale(xscale, yscale);
	iLenaScaled.save(lenaScaled);

	// scaling cameraman
	bmp::Image iCameraScaled = iCamera.scale(xscale, yscale);
	iCameraScaled.save(cameraScaled);

	// color channel modulation with 'R' channel
	bmp::Image iCornR = iCorn.resetChannel('R');
	iCornR.save(cornR);

	// color channel modulation with 'G' channel
	bmp::Image iCornG = iCorn.resetChannel('G');
	iCornG.save(cornG);

	// color channel modulation with 'B' channel
	bmp::Image iCornB = iCorn.resetChannel('B');
	iCornB.save(cornB);

	return 0;
}
