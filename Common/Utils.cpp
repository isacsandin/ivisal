#include "Utils.h"
using namespace std;


// Do the Principal Component Analysis, finding the average image
// and the eigenfaces that represent any image in the given dataset.
void Utils::doPCA(int *nEigensPointer,int nTrainFaces, IplImage ** faceImgArr, IplImage* ** eigenVectArr, 
				  IplImage* *pAvgTrainImg, CvMat* *eigenValMat){

	int i;
	CvTermCriteria calcLimit;
	CvSize faceImgSize;
	CvMat * projectedTrainFaceMat = 0; // projected training faces

	// set the number of eigenvalues to use
	int nEigens = nTrainFaces-1;
	*nEigensPointer = nEigens;

	// allocate the eigenvector images
	faceImgSize.width  = faceImgArr[0]->width;
	faceImgSize.height = faceImgArr[0]->height;
	(*eigenVectArr) = (IplImage**)cvAlloc(sizeof(IplImage*) * nEigens);
	for(i=0; i<nEigens; i++)
		(*eigenVectArr)[i] = cvCreateImage(faceImgSize, IPL_DEPTH_32F, 1);

	// allocate the eigenvalue array
	(*eigenValMat) = cvCreateMat( 1, nEigens, CV_32FC1 );

	// allocate the averaged image
	(*pAvgTrainImg) = cvCreateImage(faceImgSize, IPL_DEPTH_32F, 1);

	// set the PCA termination criterion
	calcLimit = cvTermCriteria( CV_TERMCRIT_ITER, nEigens, 1);

	// compute average image, eigenvalues, and eigenvectors
	cvCalcEigenObjects(
		nTrainFaces,
		(void*)faceImgArr,
		(void*)(*eigenVectArr),
		CV_EIGOBJ_NO_CALLBACK,
		0,
		0,
		&calcLimit,
		(*pAvgTrainImg),
		(*eigenValMat)->data.fl);

	cvNormalize(*eigenValMat, *eigenValMat, 1, 0, CV_L1, 0);
}

// Return a new image that is always greyscale, whether the input image was RGB or Greyscale.
// Remember to free the returned image using cvReleaseImage() when finished.
IplImage* Utils::convertImageToGreyscale(const IplImage *imageSrc)
{
	IplImage *imageGrey;
	// Either convert the image to greyscale, or make a copy of the existing greyscale image.
	// This is to make sure that the user can always call cvReleaseImage() on the output, whether it was greyscale or not.
	if (imageSrc->nChannels == 3) {
		imageGrey = cvCreateImage( cvGetSize(imageSrc), IPL_DEPTH_8U, 1 );
		cvCvtColor( imageSrc, imageGrey, CV_BGR2GRAY );
	}
	else {
		imageGrey = cvCloneImage(imageSrc);
	}
	return imageGrey;
}

// Creates a new image copy that is of a desired size.
// Remember to free the new image later.
IplImage* Utils::resizeImage(const IplImage *origImg, int newWidth, int newHeight)
{
	IplImage *outImg = 0;
	int origWidth;
	int origHeight;
	if (origImg) {
		origWidth = origImg->width;
		origHeight = origImg->height;
	}
	if (newWidth <= 0 || newHeight <= 0 || origImg == 0 || origWidth <= 0 || origHeight <= 0) {
		printf("ERROR in resizeImage: Bad desired image size of %dx%d\n.", newWidth, newHeight);
		exit(1);
	}

	// Scale the image to the new dimensions, even if the aspect ratio will be changed.
	outImg = cvCreateImage(cvSize(newWidth, newHeight), origImg->depth, origImg->nChannels);
	if (newWidth > origImg->width && newHeight > origImg->height) {
		// Make the image larger
		cvResetImageROI((IplImage*)origImg);
		cvResize(origImg, outImg, CV_INTER_LINEAR);	// CV_INTER_CUBIC or CV_INTER_LINEAR is good for enlarging
	}
	else {
		// Make the image smaller
		cvResetImageROI((IplImage*)origImg);
		cvResize(origImg, outImg, CV_INTER_AREA);	// CV_INTER_AREA is good for shrinking / decimation, but bad at enlarging.
	}

	return outImg;
}

// Returns a new image that is a cropped version of the original image. 
IplImage* Utils::cropImage(const IplImage *img, const CvRect region)
{
	IplImage *imageTmp;
	IplImage *imageRGB;
	CvSize size;
	size.height = img->height;
	size.width = img->width;

	if (img->depth != IPL_DEPTH_8U) {
		printf("ERROR in cropImage: Unknown image depth of %d given in cropImage() instead of 8 bits per pixel.\n", img->depth);
		exit(1);
	}

	// First create a new (color or greyscale) IPL Image and copy contents of img into it.
	imageTmp = cvCreateImage(size, IPL_DEPTH_8U, img->nChannels);
	cvCopy(img, imageTmp, NULL);

	// Create a new image of the detected region
	// Set region of interest to that surrounding the face
	cvSetImageROI(imageTmp, region);
	// Copy region of interest (i.e. face) into a new iplImage (imageRGB) and return it
	size.width = region.width;
	size.height = region.height;
	imageRGB = cvCreateImage(size, IPL_DEPTH_8U, img->nChannels);
	cvCopy(imageTmp, imageRGB, NULL);	// Copy just the region.

	cvReleaseImage( &imageTmp );
	return imageRGB;		
}

// Get an 8-bit equivalent of the 32-bit Float image.
// Returns a new image, so remember to call 'cvReleaseImage()' on the result.
IplImage* Utils::convertFloatImageToUcharImage(const IplImage *srcImg)
{
	IplImage *dstImg = 0;
	if ((srcImg) && (srcImg->width > 0 && srcImg->height > 0)) {

		// Spread the 32bit floating point pixels to fit within 8bit pixel range.
		double minVal, maxVal;
		cvMinMaxLoc(srcImg, &minVal, &maxVal);

		//cout << "FloatImage:(minV=" << minVal << ", maxV=" << maxVal << ")." << endl;

		// Deal with NaN and extreme values, since the DFT seems to give some NaN results.
		if (cvIsNaN(minVal) || minVal < -1e30)
			minVal = -1e30;
		if (cvIsNaN(maxVal) || maxVal > 1e30)
			maxVal = 1e30;
		if (maxVal-minVal == 0.0f)
			maxVal = minVal + 0.001;	// remove potential divide by zero errors.

		// Convert the format
		dstImg = cvCreateImage(cvSize(srcImg->width, srcImg->height), 8, 1);
		cvConvertScale(srcImg, dstImg, 255.0 / (maxVal - minVal), - minVal * 255.0 / (maxVal-minVal));
	}
	return dstImg;
}

// Store a greyscale floating-point CvMat image into a BMP/JPG/GIF/PNG image,
// since cvSaveImage() can only handle 8bit images (not 32bit float images).
void Utils::saveFloatImage(const char *filename, const IplImage *srcImg)
{
	//cout << "Saving Float Image '" << filename << "' (" << srcImg->width << "," << srcImg->height << "). " << endl;
	IplImage *byteImg = Utils::convertFloatImageToUcharImage(srcImg);
	cvSaveImage(filename, byteImg);
	cvReleaseImage(&byteImg);
}

// Perform face detection on the input image, using the given Haar cascade classifier.
// Returns a rectangle for the detected region in the given image.
CvRect Utils::detectFaceInImage(const IplImage *inputImg, const CvHaarClassifierCascade* cascade )
{
	const CvSize minFeatureSize = cvSize(20, 20);
	const int flags = CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_DO_ROUGH_SEARCH;	// Only search for 1 face.
	const float search_scale_factor = 1.1f;
	IplImage *detectImg;
	IplImage *greyImg = 0;
	CvMemStorage* storage;
	CvRect rc;
	double t;
	CvSeq* rects;

	storage = cvCreateMemStorage(0);
	cvClearMemStorage( storage );

	// If the image is color, use a greyscale copy of the image.
	detectImg = (IplImage*)inputImg;	// Assume the input image is to be used.
	if (inputImg->nChannels > 1) 
	{
		greyImg = cvCreateImage(cvSize(inputImg->width, inputImg->height), IPL_DEPTH_8U, 1 );
		cvCvtColor( inputImg, greyImg, CV_BGR2GRAY );
		detectImg = greyImg;	// Use the greyscale version as the input.
	}

	// Detect all the faces.
	t = (double)cvGetTickCount();
	rects = cvHaarDetectObjects( detectImg, (CvHaarClassifierCascade*)cascade, storage,
				search_scale_factor, 3, flags, minFeatureSize );
	t = (double)cvGetTickCount() - t;
	printf("[Face Detection took %d ms and found %d objects]\n", cvRound( t/((double)cvGetTickFrequency()*1000.0) ), rects->total );

	// Get the first detected face (the biggest).
	if (rects->total > 0) {
		rc = *(CvRect*)cvGetSeqElem( rects, 0 );
	}
	else
		rc = cvRect(-1,-1,-1,-1);	// Couldn't find the face.

	//cvReleaseHaarClassifierCascade( &cascade );
	//cvReleaseImage( &detectImg );
	if (greyImg)
		cvReleaseImage( &greyImg );
	cvReleaseMemStorage( &storage );

	return rc;	// Return the biggest face found, or (-1,-1,-1,-1).
}

// Read the names & image filenames of people from a text file, and load all those images listed.
int Utils::loadFaceImgArray(IplImage* ** faceImgArr, const char * filename, int *nPersons, vector<string> &personNames, CvMat* *personNumTruthMat)
{
	FILE * imgListFile = 0;
	char imgFilename[512];
	int iFace, nFaces=0;
	int i;
	
	// open the input file
	if( !(imgListFile = fopen(filename, "r")) )
	{
		fprintf(stderr, "Can\'t open file %s\n", filename);
		return 0;
	}

	// count the number of faces
	while( fgets(imgFilename, sizeof(imgFilename)-1, imgListFile) ) ++nFaces;
	rewind(imgListFile);

	// allocate the face-image array and person number matrix
	*faceImgArr        = (IplImage **)cvAlloc( nFaces*sizeof(IplImage *) );
	(*personNumTruthMat) = cvCreateMat( 1, nFaces, CV_32SC1 );

	personNames.clear();	// Make sure it starts as empty.
	*nPersons = 0;

	// store the face images in an array
	for(iFace=0; iFace<nFaces; iFace++)
	{
		char personName[256];
		string sPersonName;
		int personNumber;

		// read person number (beginning with 1), their name and the image filename.
		fscanf(imgListFile, "%d %s %s", &personNumber, personName, imgFilename);
		sPersonName = personName;
		//printf("Got %d: %d, <%s>, <%s>.\n", iFace, personNumber, personName, imgFilename);

		// Check if a new person is being loaded.
		if (personNumber > *nPersons) {
			// Allocate memory for the extra person (or possibly multiple), using this new person's name.
			for (i=*nPersons; i < personNumber; i++) {
				personNames.push_back( sPersonName );
			}
			*nPersons = personNumber;
			//printf("Got new person <%s> -> nPersons = %d [%d]\n", sPersonName.c_str(), nPersons, personNames.size());
		}

		// Keep the data
		(*personNumTruthMat)->data.i[iFace] = personNumber;

		// load the face image
		(*faceImgArr)[iFace] = cvLoadImage(imgFilename, CV_LOAD_IMAGE_GRAYSCALE);

		if( !(*faceImgArr)[iFace] )
		{
			fprintf(stderr, "Can\'t load image from %s\n", imgFilename);
			return 0;
		}
	}

	fclose(imgListFile);

	printf("Data loaded from '%s': (%d images of %d people).\n", filename, nFaces, nPersons);
	printf("People: ");
	if (*nPersons > 0)
		printf("<%s>", personNames[0].c_str());
	for (i=1; i<*nPersons; i++) {
		printf(", <%s>", personNames[i].c_str());
	}
	printf(".\n");

	return nFaces;
}
