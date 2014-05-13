#pragma once
#include "createInput.h"
#include <iostream>
#include <fstream>
#include <ostream>

using namespace std;

CreateInput::CreateInput(char* dir)
{
	this->dirPath = new char[50];
	strcpy(this->dirPath, dir);
}

CreateInput::~CreateInput(void)
{
	delete[] dirPath;
}

int CreateInput::listFiles(char* fileName) 
{
    DIR *directory = 0;
    struct dirent *entrada = 0;
    unsigned int isDir = 0x4000;
    unsigned int isFile = 0x8000;
	ofstream input;
 
    directory = opendir (this->dirPath);
 
    if (directory == 0) 
	{
        std::cerr << "Nao foi possivel abrir diretorio." << std::endl;
        return 0;
    }

	input.open(fileName);
	input.clear();

	if (input == NULL)
	{
		std::cerr << "Nao foi possivel criar o arquivo." << std::endl;
        return 0;
	}

    //Iterar sobre o diretorio
	int id = 1;
	char * acctressName = new char[50];
    while (entrada = readdir (directory))
	{
		if (entrada->d_type == isFile)
		{
			strcpy(acctressName, entrada->d_name);
			char* lastPointer = strrchr(acctressName, '.');
			*lastPointer = '\0';
			input << id << " " << acctressName << " " << this->dirPath << "\\" << entrada->d_name << std::endl;
			id++;
		}
	}
	delete [] acctressName;
    closedir (directory);
	input.close();

    return 0;
}
