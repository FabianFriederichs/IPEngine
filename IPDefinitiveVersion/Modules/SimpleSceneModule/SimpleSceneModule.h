// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PLUGIN2_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PLUGIN2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef _SIMPLESCENEMODULE_H_
#define _SIMPLESCENEMODULE_H_


#include <boost/config.hpp>
#include <ISimpleSceneModule_API.h>
#include "Scene.h"
#include <algorithm>
#include <iterator>
#include <boost/property_tree/xml_parser.hpp>
// This class is exported from the Plugin2.dll
class SimpleSceneModule : public ISimpleSceneModule_API {
public:
	SimpleSceneModule(void);
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }
	bool startUp() { m_activeScene = -1; return true; }; //do stuff?
private:
	ModuleInformation m_info;
	std::string contentmoduleidentifier = "SCM";
	std::unordered_map<ipengine::ipid, Scene> m_scenes;
	ipengine::ipid m_activeScene;
	ipengine::ipid m_maxId = std::numeric_limits<ipengine::ipid>::max();
	// Inherited via ISimpleSceneModule_API
	virtual ipengine::ipid LoadSceneFromFile(std::string filepath) override;
	virtual std::vector<ipengine::ipid> LoadSceneFromFile(std::vector<std::string>::const_iterator filepathstart, std::vector<std::string>::const_iterator filepathend) override;
	virtual bool RemoveScene(ipengine::ipid id) override;
	virtual int RemoveScene(std::vector<ipengine::ipid>::const_iterator idstart, std::vector<ipengine::ipid>::const_iterator idend) override;
	virtual bool SwitchActiveScene(ipengine::ipid id) override;
	virtual bool AddEntity(ipengine::ipid entityid, ipengine::ipid sceneid = IPID_INVALID) override;
	virtual int AddEntity(std::vector<ipengine::ipid>::const_iterator entityidstart, std::vector<ipengine::ipid>::const_iterator entityidend, ipengine::ipid sceneid = IPID_INVALID) override;
	virtual bool RemoveEntity(ipengine::ipid entityid, ipengine::ipid sceneid = IPID_INVALID) override;
	virtual int RemoveEntity(std::vector<ipengine::ipid>::const_iterator entityidstart, std::vector<ipengine::ipid>::const_iterator entityidend, ipengine::ipid sceneid=IPID_INVALID) override;
};

extern "C" BOOST_SYMBOL_EXPORT SimpleSceneModule module;
SimpleSceneModule module;
#endif // !_SIMPLESCENEMODULE_H_