#pragma once

#include <vector>
#include <string>
//#include <string.h>
#include "opencv/cv.h"
#include "opencv/cvaux.h"
#include "opencv/highgui.h"

namespace Utils
{
	// Haar Cascade file, used for Face Detection.
	static const char *faceCascadeFilename = "haarcascade_frontalface_alt.xml";

	void doPCA(int nEigens,int nTrainFaces, IplImage** faceImgArr);

	IplImage* convertImageToGreyscale(const IplImage *imageSrc);
	IplImage* resizeImage(const IplImage *origImg, int newWidth, int newHeight);
	IplImage* cropImage(const IplImage *img, const CvRect region);
	IplImage* convertFloatImageToUcharImage(const IplImage *srcImg);
	void saveFloatImage(const char *filename, const IplImage *srcImg);
	CvRect detectFaceInImage(const IplImage *inputImg, const CvHaarClassifierCascade* cascade);
	IplImage* convertFloatImageToUcharImage(const IplImage *srcImg);
}