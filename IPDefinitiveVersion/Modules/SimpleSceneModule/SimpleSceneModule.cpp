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

std::string Vec3ToString(glm::vec3 v)
{
	std::string result;
	result += std::to_string(v.x);
	result += "/";
	result += std::to_string(v.y);
	result += "/";
	result += std::to_string(v.z);
	return result;
}

std::string QuatToString(glm::quat q)
{
	std::string result;
	result += std::to_string(q.w);
	result += "/";
	result += std::to_string(q.x);
	result += "/";
	result += std::to_string(q.y);
	result += "/";
	result += std::to_string(q.z);
	return result;
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

void SimpleSceneModule::WriteSceneToFile(std::string filepath, ipengine::ipid sceneid)
{
	if (sceneid == IPID_INVALID)
		sceneid = m_activeScene;
	if(m_scenes.count(sceneid)<1)
	{
		//Scene not found
		return;
	}
	auto &scene = m_scenes[sceneid];
	
	std::unordered_map<ipengine::ipid, int> meshtointernid;
	std::unordered_map<ipengine::ipid, int> entitytointernid;
	std::unordered_map<ipengine::ipid, int> shadertointernid;
	std::unordered_map<ipengine::ipid, int> materialtointernid;
	std::unordered_map<ipengine::ipid, int> texturetointernid;

	boost::property_tree::ptree tree;
	//set up base
	auto& scenenode = tree.add("Scene", "");
	
	//Write Entities
	auto contentmodule = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>(contentmoduleidentifier);
	auto &entitiesnode = scenenode.add("Entities", "");
	int entityidcounter = 0;
	int meshidcounter = 0;
	for (auto entid : scene.getEntities())
	{
		auto entity = contentmodule->getEntityById(entid);
		auto &entitynode = entitiesnode.add("Entity", "");
		int tempid = entityidcounter;
		if (entitytointernid.count(entity->m_entityId) > 0)
			tempid = entitytointernid[entity->m_entityId];
		else
			entitytointernid[entity->m_entityId] = entityidcounter;
		entitynode.add("Id", tempid);
		auto parent = entity->m_parent;
		if (parent)
		{
			entitynode.add("ParentId", ++entityidcounter);
			entitytointernid[parent->m_entityId] = entityidcounter;
		}
		entitynode.add("StringName", entity->m_name);

		//Mesh Id
		if (contentmodule->getThreeDimEntities().count(entity->m_entityId) > 0)
		{
			auto threedentity = contentmodule->getThreeDimEntities()[entity->m_entityId];
			int tempmeshid = meshidcounter;
			if (meshtointernid.count(threedentity->m_mesheObjects->m_meshObjectId) < 1)
				meshtointernid[entity->m_entityId] = meshidcounter++;
			entitynode.add("MeshId", meshtointernid[threedentity->m_mesheObjects->m_meshObjectId]);
		}

		//Transform Data
		auto transform = entity->m_transformData.getData();
		auto &transformnode = entitynode.add("TransformData", "");
		transformnode.add("Location", Vec3ToString(transform->m_location));
		transformnode.add("Rotation", QuatToString(transform->m_rotation));
		transformnode.add("Scale", Vec3ToString(transform->m_scale));
		transformnode.add("LocalY", Vec3ToString(transform->m_localY));
		transformnode.add("LocalX", Vec3ToString(transform->m_localX));
		transformnode.add("localZ", Vec3ToString(transform->m_localZ));

		//Bounding Data
		auto bdata = entity->m_boundingData;
		auto &boundingnode = entitynode.add("BoundingData","");
		if (entity->isBoundingBox)
		{
			boundingnode.add("Center", Vec3ToString(bdata.box.m_center));
			boundingnode.add("Size", Vec3ToString(bdata.box.m_size));
			boundingnode.add("Rotation", QuatToString(bdata.box.m_rotation));
		}
		else
		{
			boundingnode.add("Center", Vec3ToString(bdata.sphere.m_center));
			boundingnode.add("Radius", std::to_string(bdata.sphere.m_radius));
		}

		//if(entity.components.size()>0) //Or something like that to check whether it is extended by components
		{
			auto typestring = "";
			entitynode.add("ExtendedType", typestring); //Replace "" with typestring from component?

			//Call Extensions for this Type
			auto& extrec = m_info.expoints;
			std::vector<ipengine::any> anyvector;
			anyvector.push_back(static_cast<ISimpleSceneModule_API*>(this));
			anyvector.push_back(typestring);
			anyvector.push_back(&entitynode);
			anyvector.push_back(entity);
			anyvector.push_back(contentmodule.get());
			anyvector.push_back(&entitytointernid);
			anyvector.push_back(&meshtointernid);
			anyvector.push_back(&shadertointernid);
			anyvector.push_back(&materialtointernid);
			anyvector.push_back(&texturetointernid);
			//SimpleSceneModule*, String, ptree*, Entity*, SimpleContentModule*, unordered_map<string, entity*
			extrec.execute("ExtendedEntity", { "this", "type", "tree","entityid", "pentity", "contentmodule", "entitymap", "meshmap", "shadermap", "materialmap", "texturemap" }, anyvector);
			//!TODO write extension for certain extended entities
		}

		entityidcounter++;
	}

	//Write Meshes
	auto &mobs = contentmodule->getMeshedObjects();
	auto &meshesnode = scenenode.add("Meshes", "");
	int materialidcounter = 0;
	for (auto &meshobj : mobs)
	{
		auto& meshnode = meshesnode.add("Mesh", "");
		if (meshtointernid.count(meshobj.m_meshObjectId) < 1)
			meshtointernid[meshobj.m_meshObjectId] = meshidcounter++;
		meshnode.add("Id", meshtointernid[meshobj.m_meshObjectId]);
		meshnode.add("Path", meshobj.filepath);
		auto &materialsnode = meshnode.add("Materials", "");

		for (auto mesh : meshobj.m_meshes)
		{
			auto &materialnode = materialsnode.add("Material", "");
			if (materialtointernid.count(mesh->m_material->m_materialId) < 1)
				materialtointernid[mesh->m_material->m_materialId] = materialidcounter++;
			materialnode.add("MaterialId", materialtointernid[mesh->m_material->m_materialId]);
		}
	}


	//Write Shaders
	auto &shaders = contentmodule->getShaders();
	auto& shadersnode = scenenode.add("Shaders", "");
	int shaderidcounter = 0;
	for (auto &shader : shaders)
	{
		auto& shadernode = shadersnode.add("Shader", "");
		if (shadertointernid.count(shader.m_shaderId) < 1)
			shadertointernid[shader.m_shaderId] = shaderidcounter++;
		shadernode.add("Id", shadertointernid[shader.m_shaderId]);
		shadernode.add("VSPath", shader.m_shaderFiles.size()>0?shader.m_shaderFiles[0]:"");
		shadernode.add("FSPath", shader.m_shaderFiles.size()>1 ? shader.m_shaderFiles[1]:"");
	}

	//Write Materials
	auto &materials = contentmodule->getMaterials();
	auto& materialsnode = scenenode.add("Materials", "");
	int texturecounter = 0;
	int materialcounter = 0;
	for (auto &mat : materials)
	{
		auto& matnode = materialsnode.add("Material", "");
		if (materialtointernid.count(mat.m_materialId) < 1)
			materialtointernid[mat.m_materialId] = materialcounter++;
		matnode.add("Id", materialtointernid[mat.m_materialId]);
		auto &texturesnode = matnode.add("Textures", "");
		for (auto &tex : mat.m_textures)
		{
			auto &texnode = texturesnode.add("Texture", "");
			if (texturetointernid.count(tex.second.m_texturefileId) < 1)
				texturetointernid[tex.second.m_texturefileId] = texturecounter++;
			texnode.add("Name", tex.first);
			texnode.add("Id", texturetointernid[tex.second.m_texturefileId]);
		}
	}
	
	//Write Textures
	auto& textures = contentmodule->getTextures();
	auto &texturesnode = scenenode.add("Textures", "");
	for (auto &tex : textures)
	{
		auto& texnode = texturesnode.add("Texture", "");
		if (texturetointernid.count(tex.m_textureId) < 1)
			texturetointernid[tex.m_textureId] = texturecounter++;
		texnode.add("Id", texturetointernid[tex.m_textureId]);
		texnode.add("TexturePath", tex.m_path);
	}

	try {
		boost::property_tree::write_xml(filepath, tree);
	}
	catch (boost::property_tree::xml_parser_error ex)
	{
		//!TODO handle stuff
	}

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

void SimpleSceneModule::cmd_load(const ipengine::ConsoleParams &params)
{
	if (params.getParamCount() == 1)
	{
		m_core->getConsole().println(("Successfuly loaded scene. Scene id: " + std::to_string(LoadSceneFromFile(params.get(0)))).c_str());
	}
	else
		m_core->getConsole().println("Please supply a scene filepath");

}

void SimpleSceneModule::cmd_write(const ipengine::ConsoleParams &params)
{
	if (params.getParamCount() == 1)
	{
		WriteSceneToFile(params.get(0), m_activeScene);
		m_core->getConsole().println("Successfuly wrote active scene to file.");
	}
	else if (params.getParamCount() == 2)
	{
		WriteSceneToFile(params.get(0), params.getInt(1));
		m_core->getConsole().println("Successfuly wrote scene to file.");
	}
	else
		m_core->getConsole().println("Please supply a filepath to save the scene to");
	//m_core->getConsole().println("Overwriting active scene")
}

bool SimpleSceneModule::_startup()
{
	m_activeScene = -1; 

	//Register sceneload and scenewrite to console
	auto& console = m_core->getConsole();
	console.addCommand("ssm.loadscene", ipengine::CommandFunc::make_func<SimpleSceneModule, &SimpleSceneModule::cmd_load>(this), "Loads scene via SimpleSceneModule. First parameter has to be a valid filepath to a scene file");
	console.addCommand("ssm.writescene", ipengine::CommandFunc::make_func<SimpleSceneModule, &SimpleSceneModule::cmd_write>(this), "Writes scene via SimpleSceneModule. First parameter has to be a valid filepath. If a file doesn't exist it will be created. Second parameter can be a scene id. Uses active scene if omitted");
	return true;
}

bool SimpleSceneModule::_shutdown()
{
	//!TODO delete all scene related entities 
	auto scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>(contentmoduleidentifier);
	auto &entities = scm->getEntities();
	for (auto& scene : m_scenes)
	{
		if (scene.first == m_activeScene)
		{
			for (auto& ent : scene.second.getEntities())
			{
				auto enttodelete = scm->getEntityById(ent);
				if(enttodelete)
					entities.erase(scm->getEntityById(ent)->m_name);
			}
		}
	}
	m_scenes.clear();
	m_activeScene = IPID_INVALID;
	return true;
}

