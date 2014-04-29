using namespace std;
#include <stdio.h>;
#include "Train.h";


int main( int argc, char** argv )
{
	Train *train;
	train = new Train();

	if( argc >= 1 ) {
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