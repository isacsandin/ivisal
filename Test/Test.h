#pragma once

#include "Common.h"

class Test
{


private:
	IplImage ** faceImgArr; // array of face images
	CvMat    *  personNumTruthMat; // array of person numbers
	vector<string> personNames;			// array of person names (indexed by the person number). Added by Shervin.
	int nPersons; // the number of people in the training set. Added by Shervin.
	int nTrainFaces; // the number of training images
	int nEigens; // the number of eigenvalues
	IplImage * pAvgTrainImg; // the average image
	IplImage ** eigenVectArr; // eigenvectors
	CvMat * eigenValMat; // eigenvalues
	CvMat * projectedTrainFaceMat; // projected training faces

public:
	Test(void);
	~Test(void);
	// Open the training data from the file 'facedata.xml'.
	int loadTrainingData(CvMat ** pTrainPersonNumMat);
	// Recognize the face in each of the test images given, and compare the results with the truth.
	void recognizeFileList(const char *szFileTest);
	// Find the most likely person based on a detection. Returns the index, and stores the confidence value into pConfidence.
	int findNearestNeighbor(float * projectedTestFace, float *pConfidence);
};

