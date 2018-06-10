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
	

	boost::shared_ptr<vr::IVRSystem> getSystem(); //Return the pointer to the initialized IVRSystem.
	bool isConnected(); //Should return true if VR_Init has been successfully called
	boost::shared_ptr<vr::IVRChaperone> getChaperone();
	boost::shared_ptr<vr::IVRCompositor> getCompositor();
	boost::shared_ptr<vr::IVROverlay> getOverlay();
	boost::shared_ptr<vr::IVRRenderModels> getRenderModels();
	boost::shared_ptr<vr::IVRScreenshots> getScreenshots();
	bool initVR();
private:
	ModuleInformation m_info;

private:
	boost::shared_ptr<vr::IVRSystem> vrsystem; //Return the pointer to the initialized IVRSystem.
	bool connected = false; //Should return true if VR_Init has been successfully called
	boost::shared_ptr<vr::IVRChaperone> chaperone;
	boost::shared_ptr<vr::IVRCompositor> compositor;
	boost::shared_ptr<vr::IVROverlay> overlay;
	boost::shared_ptr<vr::IVRRenderModels> rendermodels;
	boost::shared_ptr<vr::IVRScreenshots> screenshots;

	// Inherited via IBasicOpenVRModule_API
	virtual bool _startup() override;

	// Inherited via IBasicOpenVRModule_API
	virtual bool _shutdown() override;
};

extern "C" BOOST_SYMBOL_EXPORT BasicOpenVRModule module;
BasicOpenVRModule module;