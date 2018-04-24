// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "SimpleContentModule.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
SimpleContentModule::SimpleContentModule(void)
{
	m_info.identifier = "SimpleContentModule";
	m_info.version = "1.0";
	m_info.iam = "ISimpleContentModule_API";
	auto &mobs = getMeshedObjects();
	mobs.reserve(1000);
	auto& meshs = getMeshes();
	meshs.reserve(1000);
	getMaterials().reserve(1000);
	return;
}

//One OBJ per .obj
ipengine::ipid SimpleContentModule::addMeshFromFile(std::string path, std::string format, std::vector<ipengine::ipid> mats)
{
	if (format == "obj")
	{
		bool objloaded = false;
		bool identical = false;

		ipengine::ipid mobid = IPID_INVALID;
		for (auto &mob : getMeshedObjects())
		{
			if (mob.filepath == path)
			{
				objloaded = true;
				mobid = mob.m_meshObjectId;
				if (mats.size() == mob.meshtomaterial.size())
				{
					int counter = 0;
					for (auto meshmat : mob.meshtomaterial)
					{
						if (meshmat.second == mats[counter++])
							identical = true;
						else
						{		
							identical = false;
							break;
						}
					}
				}
				if(identical)
				{
					return mob.m_meshObjectId;
				}
				else
				{
					auto id = m_core->createID();
					getMeshedObjects().push_back(SCM::MeshedObject(mob.m_meshes, id, path));
					int meshmatindex = 0;
					for (auto mesh : mob.m_meshes)
					{
						if (meshmatindex < mats.size())
						{
							getMeshedObjectById(id)->meshtomaterial[mesh->m_meshId] = mats[meshmatindex];
							meshmatindex++;
						}
					}
					return id;
				}
			}
		}

		auto obj = OBJLoader::loadOBJ(path).objects[0];
		auto& scmmeshes = getMeshes();
		std::vector<SCM::MeshData*> meshes;
		int meshindex = 0;
		for (auto& m : obj.meshes)
		{
			OBJLoader::recalculateTangents(m);
			SCM::MeshData* data = new SCM::MeshData();
			for (auto i : m.indices)
				data->m_indices.push_back(SCM::index(i));
			for (auto v : m.vertices)
			{
				data->m_vertices.setData().push_back(SCM::VertexData(v.position, v.uv, v.normal, v.tangent));
			}
			//data->updateTangents();
			//data->m_vertices.swap(); //Does this make sense?
			data->m_meshId = m_core->createID();
			if (mats.size() <= meshindex)
			{
				//auto id = SCM::generateNewEntityId();
				//materials.push_back(SCM::MaterialData(id, -1, getDefaultShaderId()));
				if (mats.size() > 0)
				{
					auto id = mats.back();
					mats.push_back(id);
				}
			}
			auto& maters = getMaterials();
			//[id](TextureFile& a)->bool {return a.m_textureId == id; }
			meshes.push_back(data);
			scmmeshes.push_back(*data);
			meshindex++;
		}
		auto id = m_core->createID();
		getMeshedObjects().push_back(SCM::MeshedObject(meshes,id, path));
		int meshmatindex = 0;
		for (auto mesh : meshes)
		{
			if (meshmatindex < mats.size())
			{
				getMeshedObjectById(id)->meshtomaterial[mesh->m_meshId] = mats[meshmatindex];
				meshmatindex++;
			}
		}
		return id;
	}
	
	//TODO construct meshdata via use of fabians objloader
	return -1;
}

ipengine::ipid SimpleContentModule::getDefaultShaderId()
{	//TODO
	return getShaders().front().m_shaderId;
}

ipengine::ipid SimpleContentModule::generateDefaultTexture()
{
	//
	return -1;
}

ipengine::ipid SimpleContentModule::generateDefaultShader()
{
	std::string vert = "#version 330 core\nlayout(location = 0) in vec3 position;\nuniform mat4 model;\nuniform mat4 view;\nuniform mat4 projection;\nvoid main()\n{vec4 worldpos = model * vec4(position.x, position.y, position.z, 1.0);\ngl_Position = projection * view * worldpos;\n}";
	std::string frag = "#version 330 core\nout vec4 color; \nvoid main()\n{\ncolor = vec4(0.0f, 1.0f, 1.0f, 1.0f); \n}";
	std::ofstream vf, ff;
	std::string vfFname = "defaultvertexshader.vs";
	std::string ffFname = "defaultfragmentshader.fs";
	if (!std::experimental::filesystem::exists(vfFname))
	{
		//vfFname = "1" + vfFname;
		vf.open(vfFname);
		vf << vert;
		vf.close();
	}
	if (!std::experimental::filesystem::exists(ffFname))
	{
		//ffFname = "1" + ffFname;
		ff.open(ffFname);
		ff << frag;
		ff.close();
	}
	

	auto& shaders = getShaders();
	SCM::ShaderData data(m_core->createID(), vfFname, ffFname);
	shaders.insert(shaders.begin(), data);
	return data.m_shaderId;
}

ipengine::ipid SimpleContentModule::getDefaultTextureId()
{
	return getTextures().front().m_textureId;
}

bool SimpleContentModule::_startup()
{
	//Setup default material
	//auto id = SCM::m_core->createID();
	generateDefaultShader();
	generateDefaultTexture();

	m_core->getConsole().addCommand("scm.showcomponents", ipengine::CommandFunc::make_func<SimpleContentModule, &SimpleContentModule::cmd_showcomponents>(this), "Lists currently registered component types.");

	return true;
}

void SimpleContentModule::cmd_showcomponents(const ipengine::ConsoleParams& params)
{
	m_core->getConsole().println("Currently registered component types (id, name):");
	for (auto& e : componentTypes)
	{
		std::string line = std::to_string(e.second) + "\t" + e.first;
		m_core->getConsole().println(line.c_str());
	}
}

bool SimpleContentModule::_shutdown()
{
	getEntities().clear();
	getThreeDimEntities().clear();
	getDirLights().clear();
	getPointLights().clear();
	getSpotLights().clear();
	getShaders().clear();
	getMaterials().clear();
	getTextures().clear();
	getMeshes().clear();
	getMeshedObjects().clear();
	getMeshedObjects().reserve(1000);
	getMeshes().reserve(1000);
	getMaterials().reserve(1000);
	return true;
}
