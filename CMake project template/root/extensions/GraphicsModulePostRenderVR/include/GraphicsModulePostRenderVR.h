#ifndef _ExtensionTest_H_
#define _ExtensionTest_H_

#include <boost/config.hpp>
#include <IModule_API.h>
#include <ISimpleContentModule_API.h>
#include <IGraphics_API.h>
#include <IBasicOpenVRModule_API.h>
#include <GraphicsModulePreRenderVR.h>
#include <IDataStoreModuleh_API.h>

class GraphicsModulePostRenderVR : public IExtension {
public:
	GraphicsModulePostRenderVR();
	// TODO: add your methods here.
	virtual void execute(std::vector<std::string> argnames, std::vector<ipengine::any>& args) override;


	// Inherited via IExtension
	virtual ExtensionInformation * getInfo() override;
private:
	ExtensionInformation m_info;
	std::vector<ipengine::Scheduler::SubHandle> handle;
	boost::shared_ptr<IBasicOpenVRModule_API> ovrmodule;
	boost::shared_ptr<IDataStoreModuleh_API> datastore;
	boost::shared_ptr<SCM::ISimpleContentModule_API> scm;
	bool init = false;
};

extern "C" BOOST_SYMBOL_EXPORT GraphicsModulePostRenderVR extension;
GraphicsModulePostRenderVR extension;

#endif