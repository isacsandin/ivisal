#pragma once

#include "Common.h"
#include "Utils.h"

class Train
{
private:
	int nPersons;						// the number of people in the training set. Added by Shervin.
	vector<string> personNames;			// array of person names (indexed by the person number). Added by Shervin.
	int nEigens;						// the number of eigenvalues
	int nTrainFaces;				    // the number of training images
	CvMat*  personNumTruthMat;			// array of person numbers
	CvMat* eigenValMat;					// eigenvalues
	CvMat * projectedTrainFaceMat;		// projected training faces
	IplImage * pAvgTrainImg;			// the average image
	IplImage** eigenVectArr;			// eigenvectors
	

public:
	Train(void);
	~Train(void);
	int loadFaceImgArray(const char * filename);
	void learn(const char *szFileTrain);
	void storeTrainingData();
	void storeEigenfaceImages(int nEigens);
};

