#include "Test.h"

int main(int argc,char** argv)
{
	Test* test = new Test();

	if( argc >= 3 ) {
		char *fileName = argv[1];
		char *inputFileName = argv[2];
		test->recognizeImage(fileName,inputFileName);
		system("pause");
	}
	return 0;
}