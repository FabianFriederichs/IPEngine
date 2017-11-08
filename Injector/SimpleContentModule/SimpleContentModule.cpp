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
	return;
}

//One OBJ per .obj
SCM::IdType SimpleContentModule::addMeshFromFile(std::string path, std::string format)
{
	if (format == "obj")
	{
		auto obj = OBJLoader::loadOBJ(path).objects[0];
		auto& scmmeshes = getMeshes();
		std::vector<SCM::MeshData*> meshes;
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
			data->m_meshId = SCM::generateNewGeneralId();
			meshes.push_back(data);
			scmmeshes.push_back(*data);
		}
		auto id = SCM::generateNewGeneralId();
		getMeshedObjects().push_back(SCM::MeshedObject(meshes,id));
		return id;
	}
	
	//TODO construct meshdata via use of fabians objloader
	return -1;
}
