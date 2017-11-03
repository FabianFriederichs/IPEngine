#include "Scene.h"

const std::vector<SCM::EntityId> Scene::getEntities()
{
	return m_entities; //double buffer this later TO-DO
}

int Scene::addEntity(SCM::EntityId id)
{
	auto f = std::find(m_entities.begin(), m_entities.end(), id);
	if (f == m_entities.end())
	{
		m_entities.push_back(id);
		return 1; //successfully added
	}
	return 0; //didn't add because already existing
}

int Scene::removeEntity(SCM::EntityId id)
{
	auto f = std::find(m_entities.begin(), m_entities.end(), id);
	if (f != m_entities.end())
	{
		m_entities.erase(f);
		return 1; //successfully removed
	}
	return 0; //Didn't remove because element didn't exist
}
