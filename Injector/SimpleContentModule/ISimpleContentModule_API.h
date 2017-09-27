#include "IModule_API.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/quaternion.hpp"
namespace SCM
{
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
		uint32_t m_shaderId;
	};

	class MaterialData
	{
		std::string m_path;
		uint32_t m_materialId;
	};

	class TextureData
	{
		uint32_t m_textureId;
		bool m_isInMap;
		std::string m_path;
		glm::vec2 m_offset;
		glm::vec2 m_size;
		bool m_isCube;
	};

	class VertexData
	{
		glm::vec3 m_position;
		glm::vec3 m_normal;
		glm::vec3 m_tangent;
		glm::vec2 m_uv;
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

	using index = uint32_t;
	class MeshData
	{public:
		VertexVector m_vertices;
		std::vector<index> m_indices;
		MaterialData* m_material;
		uint32_t m_meshId;
	};

	class EntityData
	{public:
		Transform m_transformData;
		uint32_t m_entityId;
		virtual void swap() { m_transformData.swap(); }
	};

	class DrawableEntity : public EntityData
	{
		
	public:
		std::vector<MeshData*> m_meshes;
		EntityData* m_parent;
		virtual void swap() 
		{ 
			for (auto& mesh : m_meshes)
			{
				mesh->m_vertices.swap();
			}
		}
	};


	class ISimpleContentModule_API : public IModule_API
	{
	public:
		std::unordered_map<std::string, EntityData*> entities;
		void Swap()
		{
			for (auto& ent : entities)
			{
				ent.second->swap();
				
			}
		}








	};
}