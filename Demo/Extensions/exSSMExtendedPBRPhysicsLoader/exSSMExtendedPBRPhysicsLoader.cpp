// Plugin2.cpp : Defines the exported functions for the DLL application.
//1
#include "exSSMExtendedPBRPhysicsLoader.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
exSSMExtendedPBRPhysicsLoader::exSSMExtendedPBRPhysicsLoader()
{
	m_info.identifier = "exSSMExtendedPBRPhysicsLoader";
	m_info.depinfo.insert({ "physics", { true, false, "IPhysicsModule_API" } });
	//m_info.
}

glm::vec3 parseVectorFromString(std::string s)
{
	glm::vec3 v;
	std::string x, y, z;
	auto pos = s.find_first_of('/', 0);
	x = s.substr(0, pos);
	auto opos = pos + 1;
	pos = s.find_first_of('/', opos);
	y = s.substr(opos, pos - opos);
	opos = pos + 1;
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
	auto opos = pos + 1;
	pos = s.find_first_of('/', pos);
	x = s.substr(opos, pos - opos);
	opos = pos + 1;
	pos = s.find_first_of('/', pos);
	y = s.substr(opos, pos - opos);
	opos = pos + 1;
	pos = s.find_first_of('/', pos);
	z = s.substr(opos, pos - opos);
	q.w = std::stof(w);
	q.x = std::stof(x);
	q.y = std::stof(y);
	q.z = std::stof(z);
	return q;
}

void exSSMExtendedPBRPhysicsLoader::execute(std::vector<std::string> argnames, std::vector<ipengine::any>& args)
{

	if (argnames.size() < 1 || argnames[0] != "this")
	{
		return;
	}
	if (argnames.size() <2 || argnames[1] != "type")
	{
		return;
	}
	if (argnames.size() <3 || argnames[2] != "tree")
	{
		return;
	}if (argnames.size() <4 || argnames[3] != "entityid")
	{
		return;
	}if (argnames.size() <5 || argnames[4] != "entity")
	{
		return;
	}if (argnames.size() <6 || argnames[5] != "contentmodule")
	{
		return;
	}if (argnames.size() <7 || argnames[6] != "entitymap")
	{
		return;
	}if (argnames.size() <8 || argnames[7] != "meshmap")
	{
		return;
	}if (argnames.size() <9 || argnames[8] != "shadermap")
	{
		return;
	}if (argnames.size() <10 || argnames[9] != "materialmap")
	{
		return;
	}if (argnames.size() <11 || argnames[10] != "texturemap")
	{
		return;
	}

	if (!physics)
	{
		physics = m_info.dependencies.getDep<IPhysicsModule_API>("physics");
	}

	auto ssm = args[0].cast<ISimpleSceneModule_API*>();
	auto entid = args[3].cast<ipengine::ipid>();
	auto type = args[1].cast<std::string>();
	auto tree = args[2].cast<boost::property_tree::ptree>();
	auto entity = args[4].cast<SCM::Entity*>();
	auto scm = args[5].cast<std::shared_ptr<SCM::ISimpleContentModule_API>>();
	auto entitymap = args[6].cast<std::unordered_map<int, ipengine::ipid>*>();
	auto materialmap = args[9].cast<std::unordered_map<int, ipengine::ipid>*>();

	if (type == "ClothComponent")
	{
		IPhysicsModule_API::PhysicsContext phcontext;
		auto context = tree.find("PhysicsContext");
		if (context != tree.not_found())
		{
			//parse physics context
			phcontext.gravity = parseVectorFromString(context->second.get<std::string>("gravity", "0/-1/0"));
			phcontext.particleMass = context->second.get<float>("particleMass", 3.f);
			phcontext.particleDistance = context->second.get<float>("particleDistance", 0.8f);
			phcontext.struct_springKs = context->second.get<float>("struct_springKs", 40.f);
			phcontext.struct_springKd= context->second.get<float>("struct_springKd", 0.12f);
			phcontext.shear_springKs= context->second.get<float>("shear_springKs", 40.f);
			phcontext.shear_springKd= context->second.get<float>("shear_springKd", 0.12f);
			phcontext.bend_springKs = context->second.get<float>("bend_springKs", 30.f);
			phcontext.bend_springKd = context->second.get<float>("bend_springKd", 0.12f);
			phcontext.struct_springs= context->second.get<bool>("struct_springs", true);
			phcontext.shear_springs = context->second.get<bool>("shear_springs", true);
			phcontext.bend_springs = context->second.get<bool>("bend_springs", true);
			phcontext.use_constraints = context->second.get<bool>("use_constraints", true);
			phcontext.max_stretch = context->second.get<float>("max_stretch", 0.01f);
			phcontext.constraint_iterations = context->second.get<int>("constraint_iterations", 5);
			phcontext.airfric = context->second.get<float>("airfric", 0.3f);
			phcontext.two_pass_integration = context->second.get<bool>("two_pass_integration", true);
		}
		auto name = tree.get<std::string>("StringName", "");
		auto width = tree.get<size_t>("width", 10);
		auto height = tree.get<size_t>("height", 10);
		auto matid = tree.get<int>("materialid", -1);
		ipengine::ipid matipid = IPID_INVALID;
		if(materialmap->find(matid)!=materialmap->end())
		{
			//set matipid from matmap
			matipid = (*materialmap)[matid];
		}
		

		auto transform = SCM::Transform(entity->m_transformData);
		auto realid = physics->createCloth(name, width, height, transform, phcontext, matipid);
		
		for (auto node : tree.get_child("fixedParticles"))
		{
			auto xcord = node.second.get<long>("x", -1);
			auto ycord = node.second.get<long>("y", -1);
			if (xcord != -1 && ycord != -1)
			{
				physics->fixParticle(realid, xcord, ycord, true);
			}
		}
		(*entitymap)[entid] = realid;
	}
	else if (type == "dirlight")
	{
		auto lightcolor = parseVectorFromString(tree.get<std::string>("color", "0/1/0"));
		auto& entities = scm->getEntities();
		auto name = tree.get<std::string>("StringName", "");
		auto light = new SCM::DirectionalLight(m_core->createID(),entity->m_transformData, entity->m_boundingData, entity->isBoundingBox, false, lightcolor);
		entities[light->m_entityId] = light;
		entities[light->m_entityId]->m_name = name;
		(*entitymap)[entid] = light->m_entityId;
		scm->getDirLights().insert_or_assign(light->m_entityId, light);
	}
	else if (type == "pointlight")
	{
		auto lightcolor = parseVectorFromString(tree.get<std::string>("color", "0/1/0"));
		auto range = tree.get<float>("range", 100.f);
		auto& entities = scm->getEntities();
		auto name = tree.get<std::string>("StringName", "");
		auto light = new SCM::PointLight(m_core->createID(), entity->m_transformData, entity->m_boundingData, entity->isBoundingBox, false, lightcolor, range);
		entities[light->m_entityId] = light;
		entities[light->m_entityId]->m_name = name;
		(*entitymap)[entid] = light->m_entityId;
		scm->getPointLights().insert_or_assign(light->m_entityId, light);
	}
	else if (type == "spotlight")
	{
		auto lightcolor = parseVectorFromString(tree.get<std::string>("color", "0/1/0"));
		auto range = tree.get<float>("range", 100.f);
		auto innerCone = tree.get<float>("innerAngle", 1.f);
		auto outerCone = tree.get<float>("outerAngle", 1.f);
		auto& entities = scm->getEntities();
		auto name = tree.get<std::string>("StringName", "");
		auto light = new SCM::SpotLight(m_core->createID(), entity->m_transformData, entity->m_boundingData, entity->isBoundingBox, false, lightcolor, range, innerCone, outerCone);
		entities[light->m_entityId] = light;
		entities[light->m_entityId]->m_name = name;
		(*entitymap)[entid] = light->m_entityId;
		scm->getSpotLights().insert_or_assign(light->m_entityId,light);
	}
}

ExtensionInformation * exSSMExtendedPBRPhysicsLoader::getInfo()
{
	return &m_info;
}
