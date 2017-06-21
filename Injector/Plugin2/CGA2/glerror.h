#ifndef _GL_ERROR_
#define _GL_ERROR_
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <libheaders.h>
#include <fw_config.h>

#ifdef CGA2_DEBUG
#define GLERR printglerror(__FILE__, __LINE__);
#else
#define GLERR
#endif

void printglerror(const char* file, int line);
//check for error, print it and return true if error occured
bool checkglerror_(const char* file, int line);
	
#define checkglerror() checkglerror_(__FILE__, __LINE__)

#endif
