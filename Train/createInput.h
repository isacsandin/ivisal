#pragma once
#include "..\dirent-1.20.1\include\dirent.h";
#include <string.h>;
using namespace std;

class CreateInput
{
public:
	CreateInput(char* dir);
	~CreateInput(void);

	int CreateInput::listFiles (char* fileName);

private:
	char* dirPath;
};

