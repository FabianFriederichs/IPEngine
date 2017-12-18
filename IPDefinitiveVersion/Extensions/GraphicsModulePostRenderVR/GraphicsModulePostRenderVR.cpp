// Plugin2.cpp : Defines the exported functions for the DLL application.
//1

#include "GraphicsModulePostRenderVR.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
GraphicsModulePostRenderVR::GraphicsModulePostRenderVR()
{
	//m_info.
}


void GraphicsModulePostRenderVR::execute(std::vector<std::string> argnames, std::vector<ipengine::any>& args)
{
	int i = 0;
	if (!init)
	{
		init = true;
		scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>("SCM");
		ovrmodule = m_info.dependencies.getDep<IBasicOpenVRModule_API>("openvr");
		datastore = m_info.dependencies.getDep<IDataStoreModuleh_API>("datastore");
	}

	//Resolve right eye and then hand off to compositor
	


	//for (auto name : argnames)
	//{
	//	if (name == "this")
	//	{
	//		//Argument is calling module instance
	//		if (args.size() > i)
	//		{
	//			//(args[i].cast<GraphicsModule*>())
	//		}
	//	}
	//	if (name == "test")
	//	{
	//		if (args.size() > i)
	//		{
	//			auto scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>("SCM");
	//			auto mmeme = args[i].cast<std::string*>();
	//			args[i].cast<std::string*>()->append("Success!!");
	//		}
	//	}
	//	++i;
	//}

}

ExtensionInformation * GraphicsModulePostRenderVR::getInfo()
{
	return &m_info;
}

