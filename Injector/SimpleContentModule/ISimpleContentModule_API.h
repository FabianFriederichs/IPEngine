#ifndef _ISIMPLECONTENTMODULE_API_H_
#define _ISIMPLECONTENTMODULE_API_H_
#include "IModule_API.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/quaternion.hpp"

namespace SCM
{
	using IdType = int64_t;
	using EntityId = IdType;
	using index = uint32_t;

	
	class TransformData
	{
	public:
		TransformData() { m_isMatrixDirty = true; };
		TransformData(const TransformData& d) :m_location(d.m_location), m_rotation(d.m_rotation), m_scale(d.m_scale), m_localX(d.m_localX), m_localY(d.m_localY), m_localZ(d.m_localZ) { m_isMatrixDirty = true; };
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
		Transform(const Transform& other) : m_front(new TransformData(*other.m_front)), m_back(new TransformData(*other.m_back)) 
		{
		}
		Transform(const TransformData& data): m_front(new TransformData(data)) {
			m_back = new TransformData();
		}
		Transform& operator=(Transform other)
		{
			m_front = new TransformData(*other.m_front);
			m_back = new TransformData(*other.m_back);
			return *this;
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
		ShaderData() = default;
		ShaderData(IdType id, std::string v, std::string f):m_shaderId(id), m_shaderFiles({v,f})
		{

		}
		std::vector<std::string> m_shaderFiles;
		SCM::IdType m_shaderId;
		//Uniform descriptors maybe?
	};

	class MaterialData
	{
	public:
		MaterialData() = default;
		MaterialData(SCM::IdType id, SCM::IdType tid, SCM::IdType shaderid) : m_materialId(id), m_shaderId(shaderid), m_textureid(tid) {};
		SCM::IdType m_textureid; //array of textureids?
		SCM::IdType m_shaderId;
		SCM::IdType m_materialId;
	};

	class TextureData
	{
	public:
		TextureData() = default;
		TextureData(SCM::IdType id, std::string path, bool isCube = false, bool isInMap = false, glm::vec2 offset = { 0,0 }, glm::vec2 size = { 0,0 }) : m_textureId(id), m_path(path), m_isCube(isCube), m_isInMap(isInMap), m_offset(offset), m_size(size){};
		SCM::IdType m_textureId;
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
		VertexVector()
		{
		}
		VertexVector(const VertexVector& other) : m_front(other.m_front), m_back(other.m_back)
		{
		}
		const std::vector<VertexData>& getData() const {
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
			m_entityId = -1;
			m_parent = nullptr;
			isBoundingBox = true;
			isActive = false;
		}
		Entity(const Entity& other):m_transformData(other.m_transformData), m_parent(other.m_parent), m_name(other.m_name), m_boundingData(other.m_boundingData), isBoundingBox(other.isBoundingBox), isActive(other.isActive)
		{
			m_entityId = -1;
		}
		Entity(EntityId id, Transform& transform, BoundingData& boundingdata, bool boundingbox, bool active) :m_transformData(transform), m_boundingData(boundingdata)
		{
			m_entityId = id;
			m_parent = nullptr;
			isBoundingBox = boundingbox;
			isActive = active;
		}

		//virtual ~Entity() {};

		Transform m_transformData;
		Entity* m_parent;
		EntityId m_entityId;
		std::string m_name;
		BoundingData m_boundingData;
		bool isBoundingBox; //True for Box, False for Sphere in Union BoundingData
		bool isActive;
		//std::map<std::string, boost::any> m_decorators;
		virtual void swap() { m_transformData.swap(); }
	};

	class ThreeDimEntity : public Entity
	{

	public:
		ThreeDimEntity(EntityId id, Transform& transform, BoundingData& boundingdata, bool boundingbox, bool active, MeshedObject* meshes) :Entity(id, transform, boundingdata, boundingbox, active), m_mesheObjects(meshes)
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
				ent.second->swap();

			}
		}
		virtual std::unordered_map<std::string, Entity*>& getEntities()
		{
			return entities;
		}

		virtual std::unordered_map<EntityId, ThreeDimEntity*>& getThreeDimEntities()
		{
			return threedimentities;
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
		virtual MaterialData* getMaterialById(IdType id) 
		{ 
			auto itF = std::find_if(materials.begin(), materials.end(), [id](MaterialData& a)->bool {return a.m_materialId == id; });
			if (itF != materials.end())
			{
				return &*itF;
			}
			else
				return nullptr; 
		};
		virtual ShaderData* getShaderById(IdType id) 
		{ 
			auto itF = std::find_if(shaders.begin(), shaders.end(), [id](ShaderData& a)->bool {return a.m_shaderId == id; });
			if (itF != shaders.end())
			{
				return &*itF;
			}
			else
				return nullptr;
		};
		virtual Entity* getEntityById(EntityId id)
		{ 
			auto itF = std::find_if(entities.begin(), entities.end(), [id](std::pair<const std::string, Entity*>& a)->bool {return a.second->m_entityId== id; });
			if (itF != entities.end())
			{
				return (itF->second);
			}
			else
				return nullptr;
		};
		virtual Entity* getEntityByName(std::string name) 
		{ 
			return entities.count(name) ? entities[name] : nullptr; 
		};
		virtual MeshData* getMeshById(IdType id)
		{ 
			auto itF = std::find_if(meshes.begin(), meshes.end(), [id](MeshData& a)->bool {return a.m_meshId == id; });
			if (itF != meshes.end())
			{
				return &*itF;
			}
			else
				return nullptr;
		}
		virtual MeshedObject* getMeshedObjectById(IdType id)
		{ 
			auto itF = std::find_if(meshedobjects.begin(), meshedobjects.end(), [id](MeshedObject& a)->bool {return a.m_meshObjectId == id; });
			if (itF != meshedobjects.end())
			{
				return &*itF;
			}
			else
				return nullptr;
		}
		virtual IdType addMeshFromFile(std::string path, std::string format, std::vector<IdType> mats) = 0;
		virtual SCM::IdType getDefaultShaderId() = 0;
		virtual SCM::IdType getDefaultTextureId() = 0;

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

		std::unordered_map<std::string, Entity*> entities;
		std::unordered_map<EntityId, ThreeDimEntity*> threedimentities;
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
		return out;
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
		out += std::to_string(q.w) + " / " + std::to_string(q.x) + " / " + std::to_string(q.y) + " / " + std::to_string(q.z);
		return out;
	}

	static std::string allEntitiesAsString(ISimpleContentModule_API& content, bool withproperties = false)
	{
		std::string out="";
		for (auto ents : content.getEntities())
		{
			out += "Id: " + std::to_string(ents.second->m_entityId) + ": " + ents.first + " Active: " + (ents.second->isActive?"True":"False") + "\n";
			if (withproperties)
			{
				if (ents.second->m_parent != nullptr)
				{
					out += "\t Parent: " + std::to_string(ents.second->m_parent->m_entityId) + "\n";;
				}
				else
					out += "\t Parent: -\n";
				auto vd = ents.second->m_transformData.getData();
				out += "\t Transform: \n";
				out += "\t\t Location: " + glmvec3tostring(vd->m_location) + "\n";
				out += "\t\t Scale: " + glmvec3tostring(vd->m_scale) + "\n";
				out += "\t\t Rotation: " + glmquattostring(vd->m_rotation) + "\n";
				out += "\t\t Local X: " + glmvec3tostring(vd->m_localX) + "\n";
				out += "\t\t Local Y: " + glmvec3tostring(vd->m_localY) + "\n";
				out += "\t\t Local Z: " + glmvec3tostring(vd->m_localZ) + "\n";

				auto bd = ents.second->m_boundingData;
				if (ents.second->isBoundingBox)
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
			out += "Id: " + std::to_string(mobs.m_meshObjectId) + ": " + std::to_string(mobs.m_meshes.size()) + " meshes\n";
			if (withproperties)
			{
				for (auto m : mobs.m_meshes)
				{
					out += "    MeshId: " + std::to_string(m->m_meshId) + "\n";
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
		return out;
	}
	
}

#endif