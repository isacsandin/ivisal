#pragma once

#include <stdio.h>
#if defined WIN32 || defined _WIN32
	#include<Windows.h>
	#include <conio.h>		// For _kbhit() on Windows
	#include <direct.h>		// For mkdir(path) on Windows
	#define snprintf sprintf_s	// Visual Studio on Windows comes with sprintf_s() instead of snprintf()
#else
	#include <stdio.h>		// For getchar() on Linux
	#include <termios.h>	// For kbhit() on Linux
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/stat.h>	// For mkdir(path, options) on Linux
#endif


using namespace std;

#ifndef BOOL
	#define BOOL bool
#endif

#if defined WIN32 || defined _WIN32
	// Wrappers of kbhit() and getch() for Windows:
	#define changeKeyboardMode
	#define kbhit _kbhit
#else
	// Create an equivalent to kbhit() and getch() for Linux,
	// based on "http://cboard.cprogramming.com/c-programming/63166-kbhit-linux.html":
	
	#define VK_ESCAPE 0x1B		// Escape character

	// If 'dir' is 1, get the Linux terminal to return the 1st keypress instead of waiting for an ENTER key.
	// If 'dir' is 0, will reset the terminal back to the original settings.
	void changeKeyboardMode(int dir)
	{
		static struct termios oldt, newt;

		if ( dir == 1 ) {
			tcgetattr( STDIN_FILENO, &oldt);
			newt = oldt;
			newt.c_lflag &= ~( ICANON | ECHO );
			tcsetattr( STDIN_FILENO, TCSANOW, &newt);
		}
		else
			tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
	}

	// Get the next keypress.
	int kbhit(void)
	{
		struct timeval tv;
		fd_set rdfs;

		tv.tv_sec = 0;
		tv.tv_usec = 0;

		FD_ZERO(&rdfs);
		FD_SET (STDIN_FILENO, &rdfs);

		select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
		return FD_ISSET(STDIN_FILENO, &rdfs);
	}

	// Use getchar() on Linux instead of getch().
	#define getch() getchar()
#endif

#define USE_MAHALANOBIS_DISTANCE	// You might get better recognition accuracy if you enable this.

//#define	MAX_NAME_LENGTH 256		// Give each name a fixed size for easier code.
static int SAVE_EIGENFACE_IMAGES = 1;		// Set to 0 if you dont want images of the Eigenvectors saved to files (for debugging).





