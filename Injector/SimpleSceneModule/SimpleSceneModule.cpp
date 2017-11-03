// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "SimpleSceneModule.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
SimpleSceneModule::SimpleSceneModule()
{
	m_info.identifier = "SimpleSceneModule";
	m_info.version = "1.0";
	m_info.iam = "ISimpleSceneModule_API";
	return;
}

SimpleSceneModule::SceneId SimpleSceneModule::LoadSceneFromFile(std::string filepath)
{
	/*
	What is in a scene file?
	-Entities. By id or string (string needs helper function to convert string to ID currently)
	-Entity data such as: transform, parent id, bounding data, meshes
	-meshes have a path to an obj file or so. -> also fills material data 

	-meshes
	 -id
	 -path
	-entity
	 -id
	 -stringName
	 -meshid(-1,...)
	 -parentid(-1,...)
	 -transformdata(xyz,xyz,xyz...)
	 -boundingdata
	*/
	//TODO: path validity checks
	boost::property_tree::ptree tree;
	boost::property_tree::read_xml(filepath, tree);
	auto contentmodule = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>(contentmoduleidentifier);
	std::unordered_map<int, SCM::IdType> meshtointernid;
	//Add all meshes 
	for (auto node : tree.get_child("Scene.Meshes"))
	{
		std::string meshpath;
		int meshid;
		if (meshtointernid.count(meshid) > 0)
		{
			continue; //Skip because scenes meshid is a duplicate.
		}
		meshid = node.second.get<SCM::IdType>("Id");
		meshpath = node.second.get<std::string>("Path");
		auto pos = meshpath.find_last_of('.');
		std::string extension = pos!=std::string::npos?meshpath.substr(pos+1):"";
		meshtointernid[meshid] = contentmodule->addMeshFromFile(meshpath, extension);
	}

	for (auto node : tree.get_child("Scene.Entities"))
	{
		std::string entityname;
		int entityid;
		int meshid;
		int parentid;
		SCM::TransformData transdata;
		//TODO
		//SCM::BoundingData boundingdata;

		//if (meshtointernid.count(meshid) > 0)
		//{
		//	continue; //Skip because scenes meshid is a duplicate.
		//}
		entityid = node.second.get<SCM::IdType>("Id");
		entityname = node.second.get<std::string>("StringName");

		//auto pos = meshpath.find_last_of('.');
		//std::string extension = pos != std::string::npos ? meshpath.substr(pos + 1) : "";
		//meshtointernid[meshid] = contentmodule->addMeshFromFile(meshpath, extension);
	}

	return SceneId();
}

std::vector<SimpleSceneModule::SceneId> SimpleSceneModule::LoadSceneFromFile(std::vector<std::string>::const_iterator filepathstart, std::vector<std::string>::const_iterator filepathend)
{
	std::vector<SceneId> ids;
	for (; filepathstart < filepathend; filepathstart++)
	{
		auto c = LoadSceneFromFile(*filepathstart);
		if (c != -1)
			ids.push_back(c);
	}
	return ids;
}

bool SimpleSceneModule::RemoveScene(SceneId id)
{
	auto it = m_scenes.find(id);
	if (it != m_scenes.end())
	{
		m_scenes.erase(it);
		return true;
	}
	return false;
}

int SimpleSceneModule::RemoveScene(std::vector<SceneId>::const_iterator idstart, std::vector<SceneId>::const_iterator idend)
{
	int c;
	for (; idstart < idend; idstart++)
	{
		c += RemoveScene(*idstart) ? 1 : 0;
	}
	return c;
}

bool SimpleSceneModule::SwitchActiveScene(SceneId id)
{
	if (m_scenes.find(id) == m_scenes.end())
	{
		return false;
	}
	auto activeents = m_scenes[m_activeScene].getEntities();
	auto newents = m_scenes[id].getEntities();
	//switch active scene will need to change active state of entities in SCM

	//get set of entities not in new scene
	std::vector<SCM::EntityId> missing;
	std::set_difference(activeents.begin(), activeents.end(), newents.begin(), newents.end(), std::inserter(missing, missing.begin()));

	//get set of entities new in scene
	std::vector<SCM::EntityId> news;
	std::set_difference(newents.begin(), newents.end(), activeents.begin(), activeents.end(), std::inserter(news, news.begin()));

	auto scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>(contentmoduleidentifier);
	if (missing.size() > 0)
	{
		for (auto id : missing)
		{
			scm->getEntityById(id)->isActive = false;
		}
	}

	if (news.size() > 0)
	{
		for (auto id : missing)
		{
			scm->getEntityById(id)->isActive = true;
		}
	}

	m_activeScene = id;
}

bool SimpleSceneModule::AddEntity(SceneId sceneid, SCM::EntityId entityid)
{
	auto it = m_scenes.find(sceneid);
	if (it != m_scenes.end())
	{
		if(it->second.addEntity(entityid)==1)
			return true;
	}
	return false;
}

int SimpleSceneModule::AddEntity(SceneId sceneid, std::vector<SCM::EntityId>::const_iterator entityidstart, std::vector<SCM::EntityId>::const_iterator entityidend)
{
	int c = 0;
	for (; entityidstart < entityidend; entityidstart++)
	{
		c += AddEntity(sceneid, *entityidstart) ? 1 : 0;
	}
	return c;
}

bool SimpleSceneModule::RemoveEntity(SceneId sceneid, SCM::EntityId entityid)
{
	auto it = m_scenes.find(sceneid);
	if (it != m_scenes.end())
	{
		if (it->second.removeEntity(entityid) == 1)
			return true;
	}
	return false;
}

int SimpleSceneModule::RemoveEntity(SceneId sceneid, std::vector<SCM::EntityId>::const_iterator entityidstart, std::vector<SCM::EntityId>::const_iterator entityidend)
{
	int c=0;
	for (; entityidstart < entityidend; entityidstart++)
	{
		c += RemoveEntity(sceneid, *entityidstart) ? 1 : 0;
	}
	return c;
}
