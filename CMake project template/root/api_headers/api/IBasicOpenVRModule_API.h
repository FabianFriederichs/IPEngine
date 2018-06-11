#ifndef BASICOPENVRMODULE_H
#define BASICOPENVRMODULE_H

#include <IModule_API.h>
#include <openvr.h>
class IBasicOpenVRModule_API : public IModule_API
{
public:
	virtual boost::shared_ptr<vr::IVRSystem> getSystem() = 0; //Return the pointer to the initialized IVRSystem.
	virtual bool isConnected() = 0; //Should return true if VR_Init has been successfully called
	virtual boost::shared_ptr<vr::IVRChaperone> getChaperone() = 0;
	virtual boost::shared_ptr<vr::IVRCompositor> getCompositor() = 0;
	virtual boost::shared_ptr<vr::IVROverlay> getOverlay() = 0;
	virtual boost::shared_ptr<vr::IVRRenderModels> getRenderModels() = 0;
	virtual boost::shared_ptr<vr::IVRScreenshots> getScreenshots() = 0;
	virtual bool initVR() = 0;
	virtual void shutdown(){ vr::VR_Shutdown(); }

};


#endif // !BASICOPENVRMODULE_H
