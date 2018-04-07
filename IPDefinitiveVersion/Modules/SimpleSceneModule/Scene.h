#ifndef _SCENE_H_
#define _SCENE_H_

#include <ISimpleSceneModule_API.h>
class Scene
{
public:
	Scene():m_sceneid(IPID_INVALID) {}
	Scene(const Scene &sc) : m_entities(sc.m_entities), m_sceneid(sc.m_sceneid), filepath(sc.filepath) {}
	Scene(ipengine::ipid id) :m_sceneid(id) {};
	ipengine::ipid m_sceneid;
	std::string filepath;
	const std::vector<ipengine::ipid> getEntities();
	int addEntity(ipengine::ipid);
	int removeEntity(ipengine::ipid);
private:
	std::vector<ipengine::ipid> m_entities;
};



#endif // !#define _SCENE_H_
