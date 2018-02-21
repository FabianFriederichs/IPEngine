// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "BasicOpenVRModule.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
BasicOpenVRModule::BasicOpenVRModule()
{
	m_info.identifier = "BasicOpenVRModule";
	m_info.version = "1.0";
	m_info.iam = "IBasicOpenVRModule_API";
	return;
}

bool BasicOpenVRModule::initVR()
{
	
	vr::EVRInitError vrerr;
	auto vrs = vr::VR_Init(&vrerr, vr::EVRApplicationType::VRApplication_Scene);
	vrsystem = boost::shared_ptr<vr::IVRSystem>(vrs);
	if (!(vrerr == vr::VRInitError_None)) {
		fprintf(stderr, "OpenVR initialization failed.\n");
		vr::VR_Shutdown();
		return false;
	}
	return true;
}

bool BasicOpenVRModule::_startup()
{
	//do vr init
	if (initVR())
	{
		connected = true;
		compositor = boost::shared_ptr<vr::IVRCompositor>(vr::VRCompositor());
		chaperone = boost::shared_ptr<vr::IVRChaperone>(vr::VRChaperone());
		overlay = boost::shared_ptr<vr::IVROverlay>(vr::VROverlay());
		rendermodels = boost::shared_ptr<vr::IVRRenderModels>(vr::VRRenderModels());
		screenshots = boost::shared_ptr<vr::IVRScreenshots>(vr::VRScreenshots());
		return true;
	}

	return false;
}

boost::shared_ptr<vr::IVRSystem> BasicOpenVRModule::getSystem() //Return the pointer to the initialized IVRSystem.
{
	return vrsystem;
}
bool BasicOpenVRModule::isConnected() //Should return true if VR_Init has been successfully called
{
	return connected;
}
boost::shared_ptr<vr::IVRChaperone> BasicOpenVRModule::getChaperone()
{
	return chaperone;
}
boost::shared_ptr<vr::IVRCompositor> BasicOpenVRModule::getCompositor()
{
	return compositor;
}
boost::shared_ptr<vr::IVROverlay> BasicOpenVRModule::getOverlay()
{
	return overlay;
}
boost::shared_ptr<vr::IVRRenderModels> BasicOpenVRModule::getRenderModels()
{
	return rendermodels;
}
boost::shared_ptr<vr::IVRScreenshots> BasicOpenVRModule::getScreenshots()
{
	return screenshots;
}