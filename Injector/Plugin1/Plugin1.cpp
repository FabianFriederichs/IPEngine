// Plugin1.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Plugin1.h"

//
//// This is an example of an exported variable
//PLUGIN1_API int nPlugin1=0;
//
//// This is an example of an exported function.
//PLUGIN1_API int fnPlugin1(void)
//{
//	return 42;
//}

// This is the constructor of a class that has been exported.
// see Plugin1.h for the class definition
stdOutPrinter::stdOutPrinter()
{
	m_info.identifier = "Plugin1";
	m_info.version = "1.0";
	m_info.iam = "IPrinter_API";
	return;
}
