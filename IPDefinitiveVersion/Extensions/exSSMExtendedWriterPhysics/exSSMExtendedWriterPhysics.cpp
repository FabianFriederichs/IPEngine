// Plugin2.cpp : Defines the exported functions for the DLL application.
//1

#include "exSSMExtendedWriterPhysics.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
exSSMExtendedWriterPhysics::exSSMExtendedWriterPhysics()
{
	m_info.identifier = "exSSMExtendedWriterPhysics";

	//m_info.
}

std::string vec3ToString(glm::vec3 v)
{
	std::string result;
	result += std::to_string(v.x);
	result += "/";
	result += std::to_string(v.y);
	result += "/";
	result += std::to_string(v.z);
	return result;
}

void exSSMExtendedWriterPhysics::execute(std::vector<std::string> argnames, std::vector<ipengine::any>& args)
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
	}if (argnames.size() <5 || argnames[4] != "pentity")
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
	auto entid = args[3].cast<int>();
	auto type = args[1].cast<std::string>();
	auto tree = args[2].cast<boost::property_tree::ptree>();
	auto entity = args[4].cast<SCM::Entity*>();
	auto scm = args[5].cast<boost::shared_ptr<SCM::ISimpleContentModule_API>>();
	auto entitymap = args[6].cast<std::unordered_map<ipengine::ipid, int>*>();
	auto materialmap = args[9].cast<std::unordered_map<ipengine::ipid,int>*>();

	if (entity && type == "ClothComponent")
	{
		if (auto ccomp = entity->getComponent<IPhysicsModule_API::ClothComponent>())
		{
			auto data = ccomp->getClothData();
			auto &contextnode = tree.add("PhysicsContext", "");
			contextnode.add("gravity", vec3ToString(data.pcontext.gravity));
			contextnode.add("particleMass",std::to_string(data.pcontext.particleMass));
			contextnode.add("particleDistance", std::to_string(data.pcontext.particleDistance));
			contextnode.add("struct_springKs", std::to_string(data.pcontext.struct_springKs));
			contextnode.add("struct_springKd", std::to_string(data.pcontext.struct_springKd));
			contextnode.add("shear_springKs", std::to_string(data.pcontext.shear_springKs));
			contextnode.add("shear_springKd", std::to_string(data.pcontext.shear_springKd));
			contextnode.add("bend_springKs", std::to_string(data.pcontext.bend_springKs));
			contextnode.add("bend_springKd", std::to_string(data.pcontext.bend_springKd));
			contextnode.add("struct_springs", std::to_string(data.pcontext.struct_springs));
			contextnode.add("shear_springs", std::to_string(data.pcontext.shear_springs));
			contextnode.add("bend_springs", std::to_string(data.pcontext.bend_springs));
			contextnode.add("use_constraints", std::to_string(data.pcontext.use_constraints));
			contextnode.add("max_stretch", std::to_string(data.pcontext.max_stretch));
			contextnode.add("constraint_iterations", std::to_string(data.pcontext.constraint_iterations));
			contextnode.add("airfric", std::to_string(data.pcontext.airfric));
			contextnode.add("two_pass_integration", std::to_string(data.pcontext.two_pass_integration));
				
			tree.add("width", std::to_string(data.width));
			tree.add("height", std::to_string(data.height));

			//get material
			int matid = -1;

			//get 3dentity
			auto thrde = scm->getThreeDimEntities()[entity->m_entityId];
			if (thrde->m_mesheObjects->m_meshes.size()>0 && materialmap->count(thrde->m_mesheObjects->m_meshes.front()->m_material->m_materialId) > 0)
			{
				matid = materialmap->at(thrde->m_mesheObjects->m_meshes.front()->m_material->m_materialId);
			}
			else
			{
				//!TODO fuck
			}
			tree.add("materialid", std::to_string(matid));

			auto &fixednodes = tree.add("fixedParticles", "");
			for (auto particle : data.fixedParticles)
			{
				auto& fixnode = tree.add("fix", "");
				fixnode.add("x", std::to_string(particle.x));
				fixnode.add("y", std::to_string(particle.y));
			}
		}
	}
}

ExtensionInformation * exSSMExtendedWriterPhysics::getInfo()
{
	return &m_info;
}
