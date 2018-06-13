// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PLUGIN2_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PLUGIN2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef _SIMPLECONTENTMODULE_H_
#define _SIMPLECONTENTMODULE_H_
#include <boost/config.hpp>
#include <filesystem>
#include <ISimpleContentModule_API.h>
#include "OBJLoader.h"
// This class is exported from the Plugin2.dll
class SimpleContentModule : public SCM::ISimpleContentModule_API {
public:
	SimpleContentModule(void);
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }

																																													  // Inherited via ISimpleContentModule_API
	virtual ipengine::ipid addMeshFromFile(std::string path, std::string format, std::vector<ipengine::ipid> mats) override;
	virtual ipengine::ipid getDefaultShaderId() override;
private:
	ModuleInformation m_info;

	ipengine::ipid generateDefaultTexture();
	ipengine::ipid generateDefaultShader();

	// Inherited via ISimpleContentModule_API
	virtual ipengine::ipid getDefaultTextureId() override;

	// Inherited via ISimpleContentModule_API
	virtual bool _startup() override;
	void cmd_showcomponents(const ipengine::ConsoleParams& params);
	void cmd_recalculateBoundingVolumes(const ipengine::ConsoleParams& params);

	// Inherited via ISimpleContentModule_API
	virtual bool _shutdown() override;
};

extern "C" BOOST_SYMBOL_EXPORT SimpleContentModule module;
SimpleContentModule module;

#endif