// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "SimpleSceneModule.h"

SimpleSceneModule::SimpleSceneModule(void)
{
	m_info.identifier = "SimpleSceneModule";
	m_info.version = "1.0";
	m_info.iam = "ISimpleSceneModule_API";
	return;
}


glm::vec3& parseVectorFromString(std::string s)
{
	glm::vec3 v;
	std::string x, y, z;
	auto pos = s.find_first_of('/', 0);
	x = s.substr(0, pos);
	auto opos = pos+1;
	pos = s.find_first_of('/', opos);
	y = s.substr(opos, pos-opos);
	opos = pos+1;
	pos = s.find_first_of('/', opos);
	z = s.substr(opos, pos - opos);
	v.x = std::stof(x);
	v.y = std::stof(y);
	v.z = std::stof(z);
	return v;
}

glm::quat& parseQuatFromString(std::string s)
{
	glm::quat q;
	std::string w, x, y, z;
	auto pos = s.find_first_of('/', 0);
	w = s.substr(0, pos);
	auto opos = pos+1;
	pos = s.find_first_of('/', pos);
	x = s.substr(opos, pos - opos);
	opos = pos+1;
	pos = s.find_first_of('/', pos);
	y = s.substr(opos, pos - opos);
	opos = pos+1;
	pos = s.find_first_of('/', pos);
	z = s.substr(opos, pos - opos);
	q.w = std::stof(w);
	q.x = std::stof(x);
	q.y = std::stof(y);
	q.z = std::stof(z);
	return q;
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
	boost::property_tree::read_xml(filepath, tree,boost::property_tree::xml_parser::no_comments);
	auto contentmodule = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>(contentmoduleidentifier);
	auto& entitystorage = contentmodule->getEntities();
	std::unordered_map<int, SCM::IdType> meshtointernid;
	std::unordered_map<int, SCM::EntityId> entitytointernid;
	std::unordered_map<int, SCM::IdType> shadertointernid;
	std::unordered_map<int, SCM::IdType> materialtointernid;


	auto& shaders = contentmodule->getShaders();
	for (auto node : tree.get_child("Scene.Shaders"))
	{
		std::string fsp, vsp;
		int shaderid;
		shaderid = node.second.get<SCM::IdType>("Id", -1);
		if (shadertointernid.count(shaderid) > 0)
		{
			continue; //Skip because scenes meshid is a duplicate.
		}
		vsp = node.second.get<std::string>("VSPath", "");
		fsp = node.second.get<std::string>("FSPath", "");
		auto id = contentmodule->generateNewGeneralId();
		shaders.push_back(SCM::ShaderData(id, vsp, fsp));
		shadertointernid[shaderid] = id;
	}

	auto& materials = contentmodule->getMaterials();
	auto& textures = contentmodule->getTextures();
	for (auto node : tree.get_child("Scene.Materials"))
	{
		std::string texturepath;
		int matid, shaderid;
		matid = node.second.get<SCM::IdType>("Id", -1);
		if (materialtointernid.count(matid) > 0)
		{
			continue; //Skip because scenes meshid is a duplicate.
		}
		texturepath = node.second.get<std::string>("TexturePath", "");
		shaderid = node.second.get<SCM::IdType>("ShaderId", -1);
		if (shaderid == -1)
		{
			shaderid = contentmodule->getDefaultShaderId();
		}
		auto id = contentmodule->generateNewGeneralId();
		SCM::EntityId tid;
		if (texturepath == "")
		{
			tid = -1;//q contentmodule->getDefaultTextureId();
		}
		else
		{
			tid = contentmodule->generateNewGeneralId();
			textures.push_back(SCM::TextureData(tid, texturepath));
		}
		materials.push_back(SCM::MaterialData(id, tid, shaderid)); //TODO Textures have to be loaded somewhere
		materialtointernid[matid] = id;
	}

	//Add all meshes 
	for (auto node : tree.get_child("Scene.Meshes"))
	{
		std::string meshpath;
		int meshid;
		meshid = node.second.get<SCM::IdType>("Id", -1);
		if (meshtointernid.count(meshid) > 0)
		{
			continue; //Skip because scenes meshid is a duplicate.
		}
		meshpath = node.second.get<std::string>("Path", "");
		auto pos = meshpath.find_last_of('.');
		std::string extension = pos != std::string::npos ? meshpath.substr(pos + 1) : "";

		std::vector<SCM::IdType> materials;
		//materials
		for (auto matnode : node.second.get_child("Materials"))
		{
			int matid = matnode.second.get<SCM::IdType>("MaterialId", -1);
			materials.push_back(matid);
		}

		meshtointernid[meshid] = contentmodule->addMeshFromFile(meshpath, extension, materials);
	}


	for (auto node : tree.get_child("Scene.Entities"))
	{
		std::string entityname;
		int entityid;
		int meshid;
		SCM::TransformData transdata;
		SCM::BoundingBox boxdata;
		boxdata.m_center = glm::vec3(0, 0, 0);
		boxdata.m_size = glm::vec3(0, 0, 0);
		boxdata.m_rotation = glm::quat(0, 0, 0, 0);
		SCM::BoundingSphere spheredata;
		bool boxorsphere =true;
		//TODO

		//if (meshtointernid.count(meshid) > 0)
		//{
		//	continue; //Skip because scenes meshid is a duplicate.
		//}
		entityid = node.second.get<SCM::IdType>("Id", -1);
		if (entityid == -1 || entitytointernid.count(entityid) > 0)
		{
			continue; //Entity skipped because faulty input data or duplicate
		}
		entityname = node.second.get<std::string>("StringName", "");
		if (entityname == "" || entitystorage.count(entityname) > 0)
		{
			continue; //Entity skipped because faulty input data or duplicate
		}

		meshid = node.second.get<int>("MeshId", -1);
		transdata.m_location = parseVectorFromString(node.second.get<std::string>("TransformData.Location", "0/0/0"));
		transdata.m_rotation = parseQuatFromString(node.second.get<std::string>("TransformData.Rotation", "0/0/0/0"));
		transdata.m_scale = parseVectorFromString(node.second.get<std::string>("TransformData.Scale", "0/0/0"));
		transdata.m_localX = parseVectorFromString(node.second.get<std::string>("TransformData.LocalY", "0/0/0"));
		transdata.m_localY = parseVectorFromString(node.second.get<std::string>("TransformData.LocalX", "0/0/0"));
		transdata.m_localZ = parseVectorFromString(node.second.get<std::string>("TransformData.LocalZ", "0/0/0"));

		//Box or Sphere?
		auto boundingdata = node.second.find("BoundingData");
		if (boundingdata!= node.second.not_found())
		{
			auto spec = boundingdata->second.find("Sphere");
			if (spec != boundingdata->second.not_found())
			{
				spheredata.m_center = parseVectorFromString(spec->second.get<std::string>("Center", "0/0/0"));
				spheredata.m_radius = spec->second.get<float>("Radius", 1);
				boxorsphere = false;
			}
			else if ((spec = boundingdata->second.find("Box")) != boundingdata->second.not_found())
			{
				boxdata.m_center = parseVectorFromString(spec->second.get<std::string>("Center", "0/0/0"));
				boxdata.m_size = parseVectorFromString(spec->second.get<std::string>("Size", "0/0/0"));
				boxdata.m_rotation = parseQuatFromString(spec->second.get<std::string>("Rotation", "0/0/0/0"));
				boxorsphere = true;
			}
		}
		
		//Check Mesh id
		if (meshtointernid.find(meshid) != meshtointernid.end())
		{
			entitystorage[entityname] = new SCM::ThreeDimEntity(contentmodule->generateNewEntityId(), SCM::Transform(transdata), boxorsphere ? SCM::BoundingData(boxdata) : SCM::BoundingData(spheredata), bool(boxorsphere), false, contentmodule->getMeshedObjectById(meshtointernid[meshid]));
			entitystorage[entityname]->m_name = entityname;
			contentmodule->getThreeDimEntities()[entitystorage[entityname]->m_entityId] = static_cast<SCM::ThreeDimEntity*>(entitystorage[entityname]);
			entitytointernid[entityid] = entitystorage[entityname]->m_entityId;
		}
		else
		{
			entitystorage[entityname] = new SCM::Entity();// SCM::Transform(transdata), boxorsphere ? SCM::BoundingData(boxdata) : SCM::BoundingData(spheredata), boxorsphere, false);
			entitystorage[entityname]->m_entityId = contentmodule->generateNewEntityId();
			entitystorage[entityname]->m_boundingData = boxorsphere ? SCM::BoundingData(boxdata) : SCM::BoundingData(spheredata);
			entitystorage[entityname]->isBoundingBox = boxorsphere;
			entitystorage[entityname]->m_transformData = SCM::Transform(transdata);
			entitystorage[entityname]->m_name = entityname;
			entitytointernid[entityid] = entitystorage[entityname]->m_entityId;
		}
		//auto pos = meshpath.find_last_of('.');
		//std::string extension = pos != std::string::npos ? meshpath.substr(pos + 1) : "";
		//meshtointernid[meshid] = contentmodule->addMeshFromFile(meshpath, extension);
	}

	Scene sc(generateNewSceneId());
	m_scenes.insert({ sc.m_sceneid,sc });
	for (auto node : tree.get_child("Scene.Entities"))
	{
		int entityid;
		int parentid;
		entityid = node.second.get<SCM::IdType>("Id", -1);
		parentid = node.second.get<int>("ParentId", -1);
		if (entitytointernid.count(entityid) > 0)
		{
			m_scenes[sc.m_sceneid].addEntity(entityid);
			if (entitytointernid.count(parentid) > 0)
			{
				contentmodule->setEntityParent(entitytointernid[entityid], entitytointernid[parentid]);
			}
		}
		
	}
	return sc.m_sceneid;
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
	auto scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>(contentmoduleidentifier);
	if (m_scenes.find(m_activeScene) == m_scenes.end())
	{
		auto newents = m_scenes[id].getEntities();

		for (auto id : newents)
		{
			scm->getEntityById(id)->isActive = true;
		}
		
	}
	else
	{

	
		auto activeents = m_scenes[m_activeScene].getEntities();
		auto newents = m_scenes[id].getEntities();
		//switch active scene will need to change active state of entities in SCM

		//get set of entities not in new scene
		std::vector<SCM::EntityId> missing;
		std::set_difference(activeents.begin(), activeents.end(), newents.begin(), newents.end(), std::inserter(missing, missing.begin()));

		//get set of entities new in scene
		std::vector<SCM::EntityId> news;
		std::set_difference(newents.begin(), newents.end(), activeents.begin(), activeents.end(), std::inserter(news, news.begin()));

		
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

