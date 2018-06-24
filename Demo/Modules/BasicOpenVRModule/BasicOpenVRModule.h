// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PLUGIN2_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PLUGIN2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include <boost/config.hpp>
#include <IBasicOpenVRModule_API.h>
// This class is exported from the Plugin2.dll
class BasicOpenVRModule : public IBasicOpenVRModule_API {
public:
	BasicOpenVRModule();
	~BasicOpenVRModule(){
		shutdown();
	}
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }
	

	std::shared_ptr<vr::IVRSystem> getSystem(); //Return the pointer to the initialized IVRSystem.
	bool isConnected(); //Should return true if VR_Init has been successfully called
	std::shared_ptr<vr::IVRChaperone> getChaperone();
	std::shared_ptr<vr::IVRCompositor> getCompositor();
	std::shared_ptr<vr::IVROverlay> getOverlay();
	std::shared_ptr<vr::IVRRenderModels> getRenderModels();
	std::shared_ptr<vr::IVRScreenshots> getScreenshots();
	bool initVR();
private:
	ModuleInformation m_info;

private:
	std::shared_ptr<vr::IVRSystem> vrsystem; //Return the pointer to the initialized IVRSystem.
	bool connected = false; //Should return true if VR_Init has been successfully called
	std::shared_ptr<vr::IVRChaperone> chaperone;
	std::shared_ptr<vr::IVRCompositor> compositor;
	std::shared_ptr<vr::IVROverlay> overlay;
	std::shared_ptr<vr::IVRRenderModels> rendermodels;
	std::shared_ptr<vr::IVRScreenshots> screenshots;

	// Inherited via IBasicOpenVRModule_API
	virtual bool _startup() override;

	// Inherited via IBasicOpenVRModule_API
	virtual bool _shutdown() override;
};

extern "C" BOOST_SYMBOL_EXPORT BasicOpenVRModule module;
BasicOpenVRModule module;