using namespace std;
#include <stdio.h>;
#include "Train.h";
#include "createInput.h";

int main( int argc, char** argv )
{
	Train *train;
	train = new Train();
	if(argc >= 3)
	{
		char * dir = argv[1];
		char * fileName = argv[2];
		CreateInput* createInput = new CreateInput(dir);
		createInput->listFiles(fileName);
	}
	else if( argc == 2 ) 
	{
		char *szFileTrain;
		if (argc == 2)
			szFileTrain = argv[1];	// use the given arg
		else {
			printf("ERROR: No training file given.\n");
			return 1;
		}
		train->learn(szFileTrain);
	}

	return 0;
}