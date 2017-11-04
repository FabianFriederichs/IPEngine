#ifndef _ISIMPLECONTENTMODULE_API_H_
#define _ISIMPLECONTENTMODULE_API_H_
#include "IModule_API.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/quaternion.hpp"

namespace SCM
{
	using IdType = uint32_t;
	using EntityId = IdType;
	using index = uint32_t;

	static IdType generateNewGeneralId()
	{
		static IdType lastId = 0;
		return lastId++;
	}
	static EntityId generateNewEntityId()
	{
		static IdType lastId = 0;
		return lastId++;
	}
	class TransformData
	{
	public:
		TransformData() {};
		~TransformData() {};
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
		Transform(TransformData& data) {
			m_front = &data; m_back = new TransformData();
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

	class BoundingBox
	{
	public:
		glm::quat m_rotation;
		glm::vec3 m_center;
		glm::vec3 m_size;
	};

	class BoundingSphere
	{
	public:
		glm::vec3 m_center;
		glm::float32 m_radius;
	};

	union BoundingData
	{
		BoundingData()
		{
			box = BoundingBox();
		}
		BoundingData(BoundingBox b) :box(b) {}
		BoundingData(BoundingSphere s) : sphere(s) {}
		BoundingBox box;
		BoundingSphere sphere;
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
		VertexData(glm::vec3 pos, glm::vec2 uv, glm::vec3 norm, glm::vec3 tan) :m_position(pos), m_uv(uv), m_normal(norm), m_tangent(tan)
		{}
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
		MeshedObject(std::vector<MeshData*>& mdata, IdType id) :m_meshes(mdata), m_meshObjectId(id) {}
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
		Entity()
		{
			m_entityId = generateNewEntityId();
			isBoundingBox = true;
		}
		Entity(Transform& transform, BoundingData& boundingdata, bool boundingbox, bool active) :m_transformData(transform), m_boundingData(boundingdata)
		{
			m_entityId = generateNewEntityId();
			isBoundingBox = boundingbox;
			isActive = active;
		}
		Transform m_transformData;
		Entity* m_parent;
		EntityId m_entityId;
		BoundingData m_boundingData;
		bool isBoundingBox; //True for Box, False for Sphere in Union BoundingData
		bool isActive;
		virtual void swap() { m_transformData.swap(); }
	};

	class ThreeDimEntity : public Entity
	{

	public:
		ThreeDimEntity(Transform& transform, BoundingData& boundingdata, bool boundingbox, bool active, MeshedObject* meshes) :Entity(transform, boundingdata, boundingbox, active), m_mesheObjects(meshes)
		{
		}
		MeshedObject* m_mesheObjects;
		virtual void swap()
		{
			m_mesheObjects->swap();
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
		virtual IdType addMeshFromFile(std::string path, std::string format) = 0;
		virtual bool setEntityParent(EntityId child, EntityId parent)
		{
			auto ent = getEntityById(child);
			auto ent2 = getEntityById(parent);
			if (ent != nullptr && ent2 != nullptr)
			{
				ent->m_parent = ent2;
				return true;
			}
			return false;
		}

		//Would prefer add/remove/const get functions over returning container refs. All virtual so SCMs can change their implementation more freely
	private:

		std::unordered_map<std::string, Entity> entities;
		std::vector<ShaderData> shaders;
		std::vector<MaterialData> materials;
		std::vector<TextureData> textures;
		std::vector<MeshData> meshes;
		std::vector<MeshedObject> meshedobjects;
	};

	static std::string glmvec2tostring(glm::vec2 v)
	{
		std::string out = "";
		out += std::to_string(v.x) + " / " + std::to_string(v.y);
	}
	static std::string glmvec3tostring(glm::vec3 v)
	{
		std::string out = "";
		out += std::to_string(v.x) + " / " + std::to_string(v.y) + " / " + std::to_string(v.z);
		return out;
	}
	static std::string glmquattostring(glm::quat q)
	{
		std::string out = "";
		out += std::to_string(q.x) + " / " + std::to_string(q.y) + " / " + std::to_string(q.z);
		return out;
	}

	static std::string allEntitiesAsString(ISimpleContentModule_API& content, bool withproperties = false)
	{
		std::string out="";

		for (auto ents : content.getEntities())
		{
			out += ents.second.m_entityId + ": " + ents.first + " Active: " + (ents.second.isActive?"True":"False") + "\n";
			if (withproperties)
			{
				out += "\t Parent: " + ents.second.m_parent->m_entityId;
				auto vd = ents.second.m_transformData.getData();
				out += "\t Transform: \n";
				out += "\t\t Location: " + glmvec3tostring(vd->m_location) + "\n";
				out += "\t\t Scale: " + glmvec3tostring(vd->m_scale) + "\n";
				out += "\t\t Rotation: " + glmquattostring(vd->m_rotation) + "\n";
				out += "\t\t Local X: " + glmvec3tostring(vd->m_localX) + "\n";
				out += "\t\t Local Y: " + glmvec3tostring(vd->m_localY) + "\n";
				out += "\t\t Local Z: " + glmvec3tostring(vd->m_localZ) + "\n";

				auto bd = ents.second.m_boundingData;
				if (ents.second.isBoundingBox)
				{
					out += "\t Bounding Box: \n";
					out += "\t\t Center: " + glmvec3tostring(bd.box.m_center) + "\n";
					out += "\t\t Size: " + glmvec3tostring(bd.box.m_size) + "\n";
					out += "\t\t Rotation: " + glmquattostring(bd.box.m_rotation) + "\n";
				}
				else
				{
					out += "\t Bounding Sphere: \n";
					out += "\t\t Center: " + glmvec3tostring(bd.sphere.m_center) + "\n";
					out += "\t\t Radius: " + std::to_string(bd.sphere.m_radius) + "\n";
				}
				out += "\n\n";
			}
		}
		return out;
	}

	static std::string allMeshObjectsAsString(ISimpleContentModule_API& content, bool withproperties = false, bool extended=false)
	{
		std::string out = "";

		for (auto mobs : content.getMeshedObjects())
		{
			out += mobs.m_meshObjectId + ": " + std::to_string(mobs.m_meshes.size()) + "\n";
			if (withproperties)
			{
				for (auto m : mobs.m_meshes)
				{
					out += "\t MeshId: " + std::to_string(m->m_meshId) + "\n";
					out += "\t Indices: " + std::to_string(m->m_indices.size()) + "\n";
					if (extended)
					{
						for (auto i : m->m_indices)
						{
							out += "\t\t  " + std::to_string(i) + "\n";
						}
					}
					out += "\t Vertices: " + std::to_string(m->m_vertices.getData().size()) + "\n";
					if (extended)
					{
						for (auto v : m->m_vertices.getData())
						{
							out += "\t\t Position:" + glmvec3tostring(v.m_position);
							out += "\t\t UVs:" + glmvec2tostring(v.m_uv);
							out += "\t\t Normal:" + glmvec3tostring(v.m_normal);
							out += "\t\t Tangent:" + glmvec3tostring(v.m_tangent);
							out += "\t\t----\n";
						}
					}
				}
			}
		}
	}

}

#endif