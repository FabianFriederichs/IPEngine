// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Plugin2.h"


// This is an example of an exported variable
PLUGIN2_API int nPlugin2=0;

// This is an example of an exported function.
PLUGIN2_API int fnPlugin2(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
CPlugin2::CPlugin2()
{
	m_info.identifier = "Plugin2";
	m_info.version = "1.0";
	m_info.iam = "IModule_API";
	return;
}
