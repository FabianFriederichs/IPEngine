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


glm::vec3 parseVectorFromString(std::string s)
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

glm::quat parseQuatFromString(std::string s)
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

ipengine::ipid SimpleSceneModule::LoadSceneFromFile(std::string filepath)
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
	std::unordered_map<int, ipengine::ipid> meshtointernid;
	std::unordered_map<int, ipengine::ipid> entitytointernid;
	std::unordered_map<int, ipengine::ipid> shadertointernid;
	std::unordered_map<int, ipengine::ipid> materialtointernid;
	std::unordered_map<int, ipengine::ipid> texturetointernid;

	auto& shaders = contentmodule->getShaders();
	auto& textures = contentmodule->getTextures();
	for (auto node : tree.get_child("Scene.Textures"))
	{
		std::string path;
		int textureid;
		textureid = node.second.get<ipengine::ipid>("Id", IPID_INVALID);
		if (texturetointernid.count(textureid) > 0)
		{
			continue; //Skip because scenes meshid is a duplicate.
		}
		path = node.second.get<std::string>("TexturePath", "");
		auto id = m_core->createID();
		textures.push_back(SCM::TextureFile(id, path));
		texturetointernid[textureid] = id;
	}
	for (auto node : tree.get_child("Scene.Shaders"))
	{
		std::string fsp, vsp;
		int shaderid;
		shaderid = node.second.get<ipengine::ipid>("Id", IPID_INVALID);
		if (shadertointernid.count(shaderid) > 0)
		{
			continue; //Skip because scenes meshid is a duplicate.
		}
		vsp = node.second.get<std::string>("VSPath", "");
		fsp = node.second.get<std::string>("FSPath", "");
		auto id = m_core->createID();
		shaders.push_back(SCM::ShaderData(id, vsp, fsp));
		shadertointernid[shaderid] = id;
	}

	auto& materials = contentmodule->getMaterials();
	
	for (auto node : tree.get_child("Scene.Materials"))
	{
		int matid, shaderid;
		matid = node.second.get<ipengine::ipid>("Id", IPID_INVALID);
		std::unordered_map<std::string, int> texts;
		if (materialtointernid.count(matid) > 0)
		{
			continue; //Skip because scenes meshid is a duplicate.
		}
		//texturepath = node.second.get<std::string>("TexturePath", "");
		shaderid = node.second.get<ipengine::ipid>("ShaderId", IPID_INVALID);
		if (shaderid == -1)
		{
			shaderid = contentmodule->getDefaultShaderId();
		}
		auto id = m_core->createID();
		SCM::TextureMap tids;
		for (auto node2 : node.second.get_child("Textures"))
		{
			auto tname = node2.second.get<std::string>("Name", "");
			auto tid = node2.second.get<ipengine::ipid>("Id", IPID_INVALID);
			if (tid == ipengine::ipid(-1) || tname == "")
				continue;
			tids[tname] = SCM::TextureData(texturetointernid[tid]);
		}
		materials.push_back(SCM::MaterialData(id, shadertointernid[shaderid], tids)); //TODO Textures have to be loaded somewhere
		materialtointernid[matid] = id;
	}

	//Add all meshes 
	for (auto node : tree.get_child("Scene.Meshes"))
	{
		std::string meshpath;
		int meshid;
		meshid = node.second.get<ipengine::ipid>("Id", IPID_INVALID);
		if (meshtointernid.count(meshid) > 0)
		{
			continue; //Skip because scenes meshid is a duplicate.
		}
		meshpath = node.second.get<std::string>("Path", "");
		auto pos = meshpath.find_last_of('.');
		std::string extension = pos != std::string::npos ? meshpath.substr(pos + 1) : "";

		std::vector<ipengine::ipid> materials;
		//materials
		for (auto matnode : node.second.get_child("Materials"))
		{
			int matid = matnode.second.get<ipengine::ipid>("MaterialId", IPID_INVALID);
			if(materialtointernid.count(matid)>0)
				materials.push_back(materialtointernid[matid]);
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
		entityid = node.second.get<ipengine::ipid>("Id", IPID_INVALID);
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
		
		auto exttype = node.second.find("ExtendedType");
		if (exttype != node.second.not_found())
		{
			auto& extrec = m_info.expoints;
			std::vector<ipengine::any> anyvector;
			anyvector.push_back(static_cast<ISimpleSceneModule_API*>(this));
			auto typestring = exttype->second.get_value<std::string>();
			size_t position;
			while ((position = typestring.find("\n")) != std::string::npos)
			{
				typestring.replace(position,std::string("\n").length(), "");
			}
			while ((position = typestring.find(" ")) != std::string::npos)
			{
				typestring.replace(position, 1, "");
			}
			anyvector.push_back(typestring);
			anyvector.push_back(node.second);
			SCM::Entity tempent;// SCM::Transform(transdata), boxorsphere ? SCM::BoundingData(boxdata) : SCM::BoundingData(spheredata), boxorsphere, false);
			tempent.m_boundingData = boxorsphere ? SCM::BoundingData(boxdata) : SCM::BoundingData(spheredata);
			tempent.isBoundingBox = boxorsphere;
			tempent.m_transformData = SCM::Transform(transdata);
			tempent.m_name = entityname;
			anyvector.push_back(entityid);
			anyvector.push_back(&tempent);
			anyvector.push_back(contentmodule);
			anyvector.push_back(&entitytointernid);
			anyvector.push_back(&meshtointernid);
			anyvector.push_back(&shadertointernid);
			anyvector.push_back(&materialtointernid);
			anyvector.push_back(&texturetointernid);
			//SimpleSceneModule, String, ptree, Entity, SimpleContentModule, unordered_map<string, entity*
			extrec.execute("ExtendedEntity", { "this", "type", "tree","entityid", "entity", "contentmodule", "entitymap", "meshmap", "shadermap", "materialmap", "texturemap" }, anyvector);
		}
		else {
			//Check Mesh id
			if (meshtointernid.find(meshid) != meshtointernid.end())
			{
				//auto ntde = new SCM::ThreeDimEntity(m_core->createID(), SCM::Transform(transdata), boxorsphere ? SCM::BoundingData(boxdata) : SCM::BoundingData(spheredata), bool(boxorsphere), false, contentmodule->getMeshedObjectById(meshtointernid[meshid]));
				//ntde->generateBoundingSphere();
				entitystorage[entityname] = new SCM::ThreeDimEntity(m_core->createID(), SCM::Transform(transdata), boxorsphere ? SCM::BoundingData(boxdata) : SCM::BoundingData(spheredata), bool(boxorsphere), false, contentmodule->getMeshedObjectById(meshtointernid[meshid]));
				entitystorage[entityname]->m_name = entityname;
				contentmodule->getThreeDimEntities()[entitystorage[entityname]->m_entityId] = static_cast<SCM::ThreeDimEntity*>(entitystorage[entityname]);
				entitytointernid[entityid] = entitystorage[entityname]->m_entityId;
			}
			else
			{
				entitystorage[entityname] = new SCM::Entity();// SCM::Transform(transdata), boxorsphere ? SCM::BoundingData(boxdata) : SCM::BoundingData(spheredata), boxorsphere, false);
				entitystorage[entityname]->m_entityId = m_core->createID();
				entitystorage[entityname]->m_boundingData = boxorsphere ? SCM::BoundingData(boxdata) : SCM::BoundingData(spheredata);
				entitystorage[entityname]->isBoundingBox = boxorsphere;
				entitystorage[entityname]->m_transformData = SCM::Transform(transdata);
				entitystorage[entityname]->m_name = entityname;
				entitytointernid[entityid] = entitystorage[entityname]->m_entityId;
			}
		}
		//auto pos = meshpath.find_last_of('.');
		//std::string extension = pos != std::string::npos ? meshpath.substr(pos + 1) : "";
		//meshtointernid[meshid] = contentmodule->addMeshFromFile(meshpath, extension);
	}

	Scene sc(m_core->createID());
	m_scenes.insert({ sc.m_sceneid,sc });
	for (auto node : tree.get_child("Scene.Entities"))
	{
		ipengine::ipid entityid;
		ipengine::ipid parentid;
		entityid = node.second.get<ipengine::ipid>("Id", IPID_INVALID);
		parentid = node.second.get<ipengine::ipid>("ParentId", IPID_INVALID);
		if (entitytointernid.count(entityid)!=IPID_INVALID)
		{
			m_scenes[sc.m_sceneid].addEntity(entitytointernid[entityid]);
			if (parentid!=IPID_INVALID && entitytointernid.count(parentid)>0)
			{
				contentmodule->setEntityParent(entitytointernid[entityid], entitytointernid[parentid]);
			}
		}
		
	}
	return sc.m_sceneid;
}

std::vector<ipengine::ipid> SimpleSceneModule::LoadSceneFromFile(std::vector<std::string>::const_iterator filepathstart, std::vector<std::string>::const_iterator filepathend)
{
	std::vector<ipengine::ipid> ids;
	for (; filepathstart < filepathend; filepathstart++)
	{
		auto c = LoadSceneFromFile(*filepathstart);
		if (c != -1)
			ids.push_back(c);
	}
	return ids;
}

bool SimpleSceneModule::RemoveScene(ipengine::ipid id)
{
	auto it = m_scenes.find(id);
	if (it != m_scenes.end())
	{
		m_scenes.erase(it);
		return true;
	}
	return false;
}

int SimpleSceneModule::RemoveScene(std::vector<ipengine::ipid>::const_iterator idstart, std::vector<ipengine::ipid>::const_iterator idend)
{
	int c;
	for (; idstart < idend; idstart++)
	{
		c += RemoveScene(*idstart) ? 1 : 0;
	}
	return c;
}

bool SimpleSceneModule::SwitchActiveScene(ipengine::ipid id)
{
	if (m_scenes.find(id) == m_scenes.end())
	{
		return false;
	}
	auto scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>(contentmoduleidentifier);
	if (m_scenes.find(m_activeScene) == m_scenes.end())
	{
		auto newents = m_scenes[id].getEntities();

		for (auto ide : newents)
		{
			scm->getEntityById(ide)->isActive = true;
		}
		
	}
	else
	{

	
		auto activeents = m_scenes[m_activeScene].getEntities();
		auto newents = m_scenes[id].getEntities();
		//switch active scene will need to change active state of entities in SCM

		//get set of entities not in new scene
		std::vector<ipengine::ipid> missing;
		std::set_difference(activeents.begin(), activeents.end(), newents.begin(), newents.end(), std::inserter(missing, missing.begin()));

		//get set of entities new in scene
		std::vector<ipengine::ipid> news;
		std::set_difference(newents.begin(), newents.end(), activeents.begin(), activeents.end(), std::inserter(news, news.begin()));

		
		if (missing.size() > 0)
		{
			for (auto ide : missing)
			{
				scm->getEntityById(id)->isActive = false;
			}
		}

		if (news.size() > 0)
		{
			for (auto ide : missing)
			{
				scm->getEntityById(id)->isActive = true;
			}
		}
	}
	m_activeScene = id;
}

bool SimpleSceneModule::AddEntity(ipengine::ipid entityid, ipengine::ipid sceneid)
{
	if (sceneid == IPID_INVALID)
	{
		sceneid = m_activeScene;
	}
	auto it = m_scenes.find(sceneid);
	if (it != m_scenes.end())
	{
		if (it->second.addEntity(entityid) == 1)
		{
			if (sceneid == m_activeScene)
			{
				auto scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>(contentmoduleidentifier);

				scm->getEntityById(entityid)->isActive = true;
			}
			return true;
		}
	}

	return false;
}

int SimpleSceneModule::AddEntity(std::vector<ipengine::ipid>::const_iterator entityidstart, std::vector<ipengine::ipid>::const_iterator entityidend, ipengine::ipid sceneid)
{
	int c = 0;
	for (; entityidstart < entityidend; entityidstart++)
	{
		c += AddEntity(sceneid, *entityidstart) ? 1 : 0;
	}
	return c;
}

bool SimpleSceneModule::RemoveEntity(ipengine::ipid entityid, ipengine::ipid sceneid)
{
	if (sceneid == IPID_INVALID)
	{
		sceneid = m_activeScene;
	}
	auto it = m_scenes.find(sceneid);
	if (it != m_scenes.end())
	{
		if (it->second.removeEntity(entityid) == 1)
		{
			if (sceneid == m_activeScene)
			{
				auto scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>(contentmoduleidentifier);

				scm->getEntityById(entityid)->isActive = false;
			}
			return true;
		}
	}

	return false;
}

int SimpleSceneModule::RemoveEntity(std::vector<ipengine::ipid>::const_iterator entityidstart, std::vector<ipengine::ipid>::const_iterator entityidend, ipengine::ipid sceneid)
{
	int c=0;
	for (; entityidstart < entityidend; entityidstart++)
	{
		c += RemoveEntity(sceneid, *entityidstart) ? 1 : 0;
	}
	return c;
}

bool SimpleSceneModule::_startup()
{
	m_activeScene = -1; 
	return true;
}

