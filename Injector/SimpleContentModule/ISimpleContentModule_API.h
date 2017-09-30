#include "IModule_API.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/quaternion.hpp"
namespace SCM
{
	using IdType = uint32_t;
	using EntityId = IdType;
	using index = uint32_t;
	class TransformData
	{
	public:
		TransformData(){};
		~TransformData(){};
		glm::vec3 m_location;
		glm::quat m_rotation;
		glm::vec3 m_scale;
		glm::vec3 m_localY;
		glm::vec3 m_localX;
		glm::vec3 m_localZ;
		glm::mat4 m_transformMatrix;
		bool m_isMatrixDirty;
	private:
		
	};

	class Transform
	{
	public:
		Transform() {
			m_front = new TransformData(); m_back = new TransformData();
		}
		~Transform() {
			delete m_front; delete m_back;
		}
		const TransformData* getData() { return m_front; }
		TransformData* setData() { return m_front; }
		void swap() {
			std::swap(m_back, m_front);
		}
	private:
		TransformData* m_front;
		TransformData* m_back;
	};

	class ShaderData
	{
	public:
		std::vector<std::string> m_shaderFiles;
		IdType m_shaderId;
	};

	class MaterialData
	{
	public:
		std::string m_path;
		IdType m_materialId;
	};

	class TextureData
	{
	public:
		IdType m_textureId;
		bool m_isInMap;
		std::string m_path;
		glm::vec2 m_offset;
		glm::vec2 m_size;
		bool m_isCube;
	};

	class VertexData
	{
	public:
		glm::vec3 m_position;
		glm::vec2 m_uv;
		glm::vec3 m_normal;
		glm::vec3 m_tangent;
	};

	class VertexVector
	{
	public:
		const std::vector<VertexData>& getData() {
			return m_front;
		}
		std::vector<VertexData>& setData() {
			return m_front;
		}

		void swap() { m_front.swap(m_back); }
	private:
		std::vector<VertexData> m_front;
		std::vector<VertexData> m_back;
	};

	class MeshData
	{
	public:
		VertexVector m_vertices;
		std::vector<index> m_indices;
		MaterialData* m_material;
		IdType m_meshId;
	};

	class MeshedObject
	{
	public:
		IdType m_meshObjectId;
		std::vector<MeshData*> m_meshes;
		virtual void swap()
		{
			for (auto& mesh : m_meshes)
			{
				mesh->m_vertices.swap();
			}
		}
	};

	class Entity
	{
	public:
		Transform m_transformData;
		Entity* m_parent;
		EntityId m_entityId;
		virtual void swap() { m_transformData.swap(); }
	};

	class ThreeDimEntity : public Entity
	{
		
	public:
		MeshedObject* m_meshes;
		virtual void swap() 
		{ 
			m_meshes->swap();
		}
	};


	class ISimpleContentModule_API : public IModule_API
	{
	public:	
		virtual void Swap()
		{
			for (auto& ent : entities)
			{
				ent.second.swap();
				
			}
		}
		virtual std::unordered_map<std::string, Entity>& getEntities()
		{
			return entities;
		}

		virtual std::vector<ShaderData>& getShaders()
		{
			return shaders;
		}

		virtual std::vector<MaterialData>& getMaterials()
		{
			return materials;
		}

		virtual std::vector<TextureData>& getTextures()
		{
			return textures;
		}

		virtual std::vector<MeshData>& getMeshes()
		{
			return meshes;
		}

		virtual std::vector<MeshedObject>& getMeshedObjects()
		{
			return meshedobjects;
		}

		virtual TextureData* getTextureById(IdType id)
		{
			auto itF = std::find_if(textures.begin(), textures.end(), [id](TextureData& a)->bool {return a.m_textureId == id; });
			if (itF != textures.end())
			{
				return &*itF;
			}
			else
				return nullptr;
		}
		virtual MaterialData* getMaterialById(IdType id) { return nullptr; };
		virtual ShaderData* getShaderById(IdType id) { return nullptr; };
		virtual Entity* getEntityById(IdType id) { return nullptr; };
		virtual Entity* getEntityByName(std::string name) { return entities.count(name) ? &entities[name] : nullptr; };
		virtual MeshData* getMeshById(IdType id) { return nullptr; }
		virtual MeshedObject* getMeshedObjectById(IdType id) { return nullptr; }
		//Would prefer add/remove/const get functions over returning container refs. All virtual so SCMs can change their implementation more freely
	private:
		std::unordered_map<std::string, Entity> entities;
		std::vector<ShaderData> shaders;
		std::vector<MaterialData> materials;
		std::vector<TextureData> textures;
		std::vector<MeshData> meshes;
		std::vector<MeshedObject> meshedobjects;
	};
}