#include "Train.h"

Train::Train(void)
{
	nPersons = 0;
	nEigens   = 0;
	nTrainFaces = 0;
	personNumTruthMat = 0;
	projectedTrainFaceMat = 0;
	pAvgTrainImg = 0;
	eigenVectArr = 0;
}


Train::~Train(void)
{
}

// Train from the data in the given text file, and store the trained data into the file 'facedata.xml'.
void Train::learn(const char *szFileTrain)
{
	int i, 
		offset;
	
	IplImage ** faceImgArr        = 0; // array of face images

	// load training data
	printf("Loading the training images in '%s'\n", szFileTrain);
	nTrainFaces = Utils::loadFaceImgArray(&faceImgArr, szFileTrain, &nPersons, personNames, &personNumTruthMat);

	printf("Got %d training images.\n", nTrainFaces);
	if( nTrainFaces < 2 )
	{
		fprintf(stderr,
		        "Need 2 or more training faces\n"
		        "Input file contains only %d\n", nTrainFaces);
		return;
	}

	// do PCA on the training faces
	Utils::doPCA(&nEigens, nTrainFaces, faceImgArr, &eigenVectArr, &pAvgTrainImg, &eigenValMat);

	// project the training images onto the PCA subspace
	projectedTrainFaceMat = cvCreateMat( nTrainFaces, nEigens, CV_32FC1 );
	offset = projectedTrainFaceMat->step / sizeof(float);
	for(i=0; i<nTrainFaces; i++)
	{
		//int offset = i * nEigens;
		cvEigenDecomposite(
			faceImgArr[i],
			nEigens,
			eigenVectArr,
			0, 0,
			pAvgTrainImg,
			//projectedTrainFaceMat->data.fl + i*nEigens);
			projectedTrainFaceMat->data.fl + i*offset);
	}

	// store the recognition data as an xml file
	storeTrainingData();

	// Save all the eigenvectors as images, so that they can be checked.
	if (SAVE_EIGENFACE_IMAGES) {
		storeEigenfaceImages(nEigens);
	}

}

// Save the training data to the file 'facedata.xml'.
void Train::storeTrainingData()
{
	CvFileStorage * fileStorage;
	int i;

	// create a file-storage interface
	fileStorage = cvOpenFileStorage( "facedata.xml", 0, CV_STORAGE_WRITE );

	// Store the person names. Added by Shervin.
	cvWriteInt( fileStorage, "nPersons", nPersons );
	for (i=0; i<nPersons; i++) {
		char varname[200];
		snprintf( varname, sizeof(varname)-1, "personName_%d", (i+1) );
		cvWriteString(fileStorage, varname, personNames[i].c_str(), 0);
	}

	// store all the data
	cvWriteInt( fileStorage, "nEigens", nEigens );
	cvWriteInt( fileStorage, "nTrainFaces", nTrainFaces );
	cvWrite(fileStorage, "trainPersonNumMat", personNumTruthMat, cvAttrList(0,0));
	cvWrite(fileStorage, "eigenValMat", eigenValMat, cvAttrList(0,0));
	cvWrite(fileStorage, "projectedTrainFaceMat", projectedTrainFaceMat, cvAttrList(0,0));
	cvWrite(fileStorage, "avgTrainImg", pAvgTrainImg, cvAttrList(0,0));
	for(i=0; i<nEigens; i++)
	{
		char varname[200];
		snprintf( varname, sizeof(varname)-1, "eigenVect_%d", i );
		cvWrite(fileStorage, varname, eigenVectArr[i], cvAttrList(0,0));
	}

	// release the file-storage interface
	cvReleaseFileStorage( &fileStorage );
}


// Save all the eigenvectors as images, so that they can be checked.
void Train::storeEigenfaceImages(int nEigens)
{

	// Store the average image to a file
	printf("Saving the image of the average face as 'out_averageImage.bmp'.\n");
	cvSaveImage("out_averageImage.bmp", pAvgTrainImg);
	// Create a large image made of many eigenface images.
	// Must also convert each eigenface image to a normal 8-bit UCHAR image instead of a 32-bit float image.
	printf("Saving the %d eigenvector images as 'out_eigenfaces.bmp'\n", nEigens);
	if (nEigens > 0) {
		// Put all the eigenfaces next to each other.
		int COLUMNS = 8;	// Put upto 8 images on a row.
		int nCols = min(nEigens, COLUMNS);
		int nRows = 1 + (nEigens / COLUMNS);	// Put the rest on new rows.
		int w = eigenVectArr[0]->width;
		int h = eigenVectArr[0]->height;
		CvSize size;
		size = cvSize(nCols * w, nRows * h);
		IplImage *bigImg = cvCreateImage(size, IPL_DEPTH_8U, 1);	// 8-bit Greyscale UCHAR image
		for (int i=0; i<nEigens; i++) {
			// Get the eigenface image.
			IplImage *byteImg = Utils::convertFloatImageToUcharImage(eigenVectArr[i]);
			// Paste it into the correct position.
			int x = w * (i % COLUMNS);
			int y = h * (i / COLUMNS);
			CvRect ROI = cvRect(x, y, w, h);
			cvSetImageROI(bigImg, ROI);
			cvCopyImage(byteImg, bigImg);
			cvResetImageROI(bigImg);
			cvReleaseImage(&byteImg);
		}
		cvSaveImage("out_eigenfaces.bmp", bigImg);
		cvReleaseImage(&bigImg);
	}
}
