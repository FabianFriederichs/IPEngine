// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "VRGraphicsModule.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
VRGraphicsModule::VRGraphicsModule()
{
	m_info.identifier = "VRGraphicsModule";
	m_info.version = "1.0";
	m_info.iam = "IGraphics_API";
	return;
}

bool VRGraphicsModule::startUp()
{
	return true;
}


void VRGraphicsModule::render()
{
	//stuff
}