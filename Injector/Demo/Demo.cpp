// Demo.cpp : Defines the entry point for the console application.
//
#include <tchar.h>
#include "Injector.h"
int _tmain(int argc, _TCHAR* argv[])
{
	Injector inj("XMLFile.xml", "../Debug");
	inj.LoadModules();
	std::string meme;
	std::cin >> meme;
	return 0;
}

