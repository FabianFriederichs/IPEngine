#ifndef _ISIMPLESCENEMODULE_API_H_
#define _ISIMPLESCENEMODULE_API_H_
#include "IModule_API.h"
#include "ISimpleContentModule_API.h"

class ISimpleSceneModule_API : public IModule_API
{
public:
	using SceneId = uint32_t;

	static SceneId generateNewSceneId()
	{
		static SceneId lastId = 0;
		return lastId++;
	}

	//Load scene file, return id
	virtual SceneId LoadSceneFromFile(std::string filepath) = 0;
	virtual std::vector<SceneId> LoadSceneFromFile(std::vector<std::string>::const_iterator filepathstart, std::vector<std::string>::const_iterator filepathend) = 0;

	virtual bool RemoveScene(SceneId id) = 0;
	virtual int RemoveScene(std::vector<SceneId>::const_iterator idstart, std::vector<SceneId>::const_iterator idend) = 0;

	//Switch scene, take scene id
	virtual bool SwitchActiveScene(SceneId id) = 0;

	//Edit scene, add/remove entity?
	virtual bool AddEntity(SceneId sceneid, SCM::EntityId entityid) = 0;
	virtual int AddEntity(SceneId sceneid, std::vector<SCM::EntityId>::const_iterator entityidstart, std::vector<SCM::EntityId>::const_iterator  entityidend) = 0;

	virtual bool RemoveEntity(SceneId sceneid, SCM::EntityId entityid) = 0;
	virtual int RemoveEntity(SceneId sceneid, std::vector<SCM::EntityId>::const_iterator entityidstart, std::vector<SCM::EntityId>::const_iterator  entityidend) = 0;
};

#endif// !_ISIMPLESCENEMODULE_API_H_
