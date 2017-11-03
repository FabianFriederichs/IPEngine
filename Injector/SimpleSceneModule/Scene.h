#ifndef _SCENE_H_
#define _SCENE_H_

#include "ISimpleSceneModule_API.h"
class Scene
{
public:
	const ISimpleSceneModule_API::SceneId m_sceneid;

	const std::vector<SCM::EntityId> getEntities();
	int addEntity(SCM::EntityId);
	int removeEntity(SCM::EntityId);
private:
	std::vector<SCM::EntityId> m_entities;
};



#endif // !#define _SCENE_H_
