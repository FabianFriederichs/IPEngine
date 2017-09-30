#include "IModule_API.h"
#include "ISimpleContentModule_API.h"
class ISimpleSceneModule_API : public IModule_API
{
public:
	using SceneId = uint32_t;
	//Load scene file, return id
	virtual SceneId LoadSceneFromFile(std::string filepath) = 0;
	virtual SceneId LoadSceneFromFile(std::vector<std::string>::const_iterator filepathstart, std::vector<std::string>::const_iterator filepathend) = 0;

	virtual bool RemoveScene(SceneId id) = 0;
	virtual bool RemoveScene(std::vector<SCM::EntityId>::const_iterator idstart, std::vector<SCM::EntityId>::const_iterator idend) = 0;

	//Switch scene, take scene id
	virtual void SwitchActiveScene(SceneId id) = 0;

	//Edit scene, add/remove entity?
	virtual bool AddEntity(SceneId sceneid, SCM::EntityId entityid) = 0;
	virtual bool AddEntity(SceneId sceneid, std::vector<SCM::EntityId>::const_iterator entityidstart, std::vector<SCM::EntityId>::const_iterator  entityidend) = 0;

	virtual bool RemoveEntity(SceneId sceneid, SCM::EntityId entityid) = 0;
	virtual bool RemoveEntity(SceneId sceneid, std::vector<SCM::EntityId>::const_iterator entityidstart, std::vector<SCM::EntityId>::const_iterator  entityidend) = 0;
};