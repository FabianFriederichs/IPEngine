// Plugin2.cpp : Defines the exported functions for the DLL application.
//1

#include "VREntitySceneLoader.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
VREntitySceneLoader::VREntitySceneLoader()
{
	//m_info.
}



void VREntitySceneLoader::execute(std::vector<std::string> argnames, std::vector<ipengine::any>& args)
{
	int i = 0;
	vr::EVRInitError vrerr;
	if (!init)
	{
		init = true;
		scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>("SCM");
		ovrmodule = m_info.dependencies.getDep<IBasicOpenVRModule_API>("openvr");
		scenemodule = m_info.dependencies.getDep<ISimpleSceneModule_API>("scene");
	}


}

ExtensionInformation * VREntitySceneLoader::getInfo()
{
	return &m_info;
}
