// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "GraphicsModule.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
GraphicsModule::GraphicsModule()
{
	m_info.identifier = "GraphicsModule";
	m_info.version = "1.0";
	m_info.iam = "IGraphics_API";
	return;
}
