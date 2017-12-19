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
	getMeshedObjects().reserve(1000);
	getMeshes().reserve(1000);
	return;
}

bool SimpleContentModule::startUp()
{
	//Setup default material
	//auto id = SCM::generateNewGeneralId();
	generateDefaultShader();
	generateDefaultTexture();

	return true;
}

//One OBJ per .obj
SCM::IdType SimpleContentModule::addMeshFromFile(std::string path, std::string format, std::vector<SCM::IdType> mats)
{
	if (format == "obj")
	{
		auto obj = OBJLoader::loadOBJ(path).objects[0];
		auto& scmmeshes = getMeshes();
		std::vector<SCM::MeshData*> meshes;
		int meshindex = 0;
		for (auto m : obj.meshes)
		{
			SCM::MeshData* data = new SCM::MeshData();
			for (auto i : m.indices)
				data->m_indices.push_back(SCM::index(i));
			for (auto v : m.vertices)
			{
				data->m_vertices.setData().push_back(SCM::VertexData(v.position, v.uv, v.normal, v.tangent));
			}
			//data->m_vertices.swap(); //Does this make sense?
			data->m_meshId = generateNewGeneralId();
			if (mats.size() <= meshindex)
			{
				auto & materials = getMaterials();

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
			data->m_material = &*std::find_if(maters.begin(), maters.end(), [mats, meshindex](SCM::MaterialData& m)->bool { return m.m_materialId == mats[meshindex]; });
			meshes.push_back(data);
			scmmeshes.push_back(*data);
			meshindex++;
		}
		auto id = generateNewGeneralId();
		getMeshedObjects().push_back(SCM::MeshedObject(meshes,id));
		return id;
	}
	
	//TODO construct meshdata via use of fabians objloader
	return -1;
}

SCM::IdType SimpleContentModule::getDefaultShaderId()
{	//TODO
	return getShaders().front().m_shaderId;
}

SCM::IdType SimpleContentModule::generateDefaultTexture()
{
	//
	return -1;
}

SCM::IdType SimpleContentModule::generateDefaultShader()
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
	SCM::ShaderData data(generateNewGeneralId(), vfFname, ffFname);
	shaders.insert(shaders.begin(), data);
	return data.m_shaderId;
}

SCM::IdType SimpleContentModule::getDefaultTextureId()
{
	return getTextures().front().m_textureId;
}
