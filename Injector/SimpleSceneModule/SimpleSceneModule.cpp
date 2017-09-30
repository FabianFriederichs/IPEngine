// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "SimpleSceneModule.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
SimpleSceneModule::SimpleSceneModule()
{
	m_info.identifier = "SimpleSceneModule";
	m_info.version = "1.0";
	m_info.iam = "ISimpleSceneModule_API";
	return;
}
