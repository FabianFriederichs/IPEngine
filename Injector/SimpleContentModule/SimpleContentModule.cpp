// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "SimpleContentModule.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
SimpleContentModule::SimpleContentModule()
{
	m_info.identifier = "SimpleContentModule";
	m_info.version = "1.0";
	m_info.iam = "ISimpleContentModule_API";
	return;
}
