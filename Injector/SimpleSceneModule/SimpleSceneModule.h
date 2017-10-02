// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PLUGIN2_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PLUGIN2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include <boost/config.hpp>
#include "ISimpleSceneModule_API.h"
// This class is exported from the Plugin2.dll
class SimpleSceneModule : public ISimpleSceneModule_API {
public:
	SimpleSceneModule(void);
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }
	bool startUp(){/*	m_info.dependencies.getDep<IPrinter_API>("printer")->printStuffToSomething(m_info.identifier + " successfully started up as " + m_info.iam); return true; */} //do stuff?
private:
	ModuleInformation m_info;
	std::unordered_map<SceneId, Scene> m_scenes;

	// Inherited via ISimpleSceneModule_API
	virtual SceneId LoadSceneFromFile(std::string filepath) override;
	virtual SceneId LoadSceneFromFile(std::vector<std::string>::const_iterator filepathstart, std::vector<std::string>::const_iterator filepathend) override;
	virtual bool RemoveScene(SceneId id) override;
	virtual bool RemoveScene(std::vector<SCM::EntityId>::const_iterator idstart, std::vector<SCM::EntityId>::const_iterator idend) override;
	virtual void SwitchActiveScene(SceneId id) override;
	virtual bool AddEntity(SceneId sceneid, SCM::EntityId entityid) override;
	virtual bool AddEntity(SceneId sceneid, std::vector<SCM::EntityId>::const_iterator entityidstart, std::vector<SCM::EntityId>::const_iterator entityidend) override;
	virtual bool RemoveEntity(SceneId sceneid, SCM::EntityId entityid) override;
	virtual bool RemoveEntity(SceneId sceneid, std::vector<SCM::EntityId>::const_iterator entityidstart, std::vector<SCM::EntityId>::const_iterator entityidend) override;
};

extern "C" BOOST_SYMBOL_EXPORT SimpleSceneModule module;
SimpleSceneModule module;