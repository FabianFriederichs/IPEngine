#ifndef _GL_ERROR_
#define _GL_ERROR_
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "libheaders.h"
#define THROW_ON_GL_ERROR 1		//throw exception when OpenGL error occures
#define HOLD_ON_GL_ERROR 0		//print error and wait for a key when OpenGL error occures
#define LOG_GL_ERRORS 1	

#define GLERR printglerror(__FILE__, __LINE__);


void printglerror(const char* file, int line);
//check for error, print it and return true if error occured
bool checkglerror_(const char* file, int line);
	
#define checkglerror() checkglerror_(__FILE__, __LINE__)

#endif
