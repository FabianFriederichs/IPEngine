// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "ModulCoreTest.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
ModulCoreTest::ModulCoreTest()
{
	m_info.identifier = "ModulCoreTest";
	m_info.version = "1.0";
	m_info.iam = "IModulCoreTest_API";
	return;
}
