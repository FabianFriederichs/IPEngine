#ifndef _ISIMPLECONTENTMODULE_API_H_
#define _ISIMPLECONTENTMODULE_API_H_
#include <IModule_API.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>


namespace SCM
{
	class TextureData;
	using index = uint32_t;
	using TextureMap = std::unordered_map<std::string, TextureData>;
	
	class TransformData
	{
	public:
		//TransformData() { m_isMatrixDirty = true; };
		//TransformData(const TransformData& d) :m_location(d.m_location), m_rotation(d.m_rotation), m_scale(d.m_scale), m_localX(d.m_localX), m_localY(d.m_localY), m_localZ(d.m_localZ) { m_isMatrixDirty = true; };
		TransformData():
			m_location(0.0f),
			m_rotation(),
			m_scale(1.0f),
			m_localX(1.0f, 0.0f, 0.0f),
			m_localY(0.0f, 1.0f, 0.0f),
			m_localZ(0.0f, 0.0f, 1.0f),
			m_transformMatrix(),
			m_isMatrixDirty(true)
		{}

		TransformData(
			const glm::vec3& location,
			const glm::quat& rotation,
			const glm::vec3& scale):
				m_location(location),
				m_rotation(rotation),
				m_scale(scale),
				m_isMatrixDirty(true)
		{
			updateTransform();
		}

		TransformData(
			const glm::mat4& transformMatrix) :
			m_location(transformMatrix[3][0], transformMatrix[3][1], transformMatrix[3][2]),
			m_rotation(glm::toQuat(glm::mat3(glm::normalize(transformMatrix[0]),
											 glm::normalize(transformMatrix[1]),
											 glm::normalize(transformMatrix[2])))),
			m_scale(glm::length(transformMatrix[0]),
					glm::length(transformMatrix[1]),
					glm::length(transformMatrix[2])),
			m_transformMatrix(transformMatrix),
			m_isMatrixDirty(false)
		{
			calcLocalAxes();
		}

		TransformData(const TransformData& other) = default;
		TransformData(TransformData&& other) = default;

		TransformData& operator=(const TransformData& other) = default;
		TransformData& operator=(TransformData&& other) = default;

		~TransformData() {};
		glm::vec3 m_location;
		glm::quat m_rotation;
		glm::vec3 m_scale;
		glm::vec3 m_localY;
		glm::vec3 m_localX;
		glm::vec3 m_localZ;
		glm::mat4 m_transformMatrix;
		bool m_isMatrixDirty;

		void calcTransformMatrix()
		{
			glm::mat4 T = glm::translate(m_location);
			glm::mat4 R = glm::toMat4(m_rotation);
			glm::mat4 S = glm::scale(m_scale);
			
			m_transformMatrix = T * R * S;
		}

		void calcLocalAxes()
		{
			m_localX = glm::normalize(glm::vec3(m_transformMatrix[0][0], m_transformMatrix[0][1], m_transformMatrix[0][2]));
			m_localY = glm::normalize(glm::vec3(m_transformMatrix[1][0], m_transformMatrix[1][1], m_transformMatrix[1][2]));
			m_localZ = glm::normalize(glm::vec3(m_transformMatrix[2][0], m_transformMatrix[2][1], m_transformMatrix[2][2]));			
		}

		void updateTransform()
		{
			calcTransformMatrix();
			calcLocalAxes();
			m_isMatrixDirty = false;
		}		
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
			m_back = new TransformData(data);
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
		const TransformData* getData() { return m_back; }
		TransformData* setData() { return m_back; }
		void swap() {
			//std::swap(m_back, m_front);
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
		ShaderData(ipengine::ipid id, std::string v, std::string f):m_shaderId(id), m_shaderFiles({v,f})
		{

		}
		std::vector<std::string> m_shaderFiles;
		ipengine::ipid m_shaderId;
		//Uniform descriptors maybe?
	};

	class MaterialData
	{
	public:
		
		MaterialData() = default;
		MaterialData(ipengine::ipid id, ipengine::ipid shaderid, const TextureMap &textures) : m_materialId(id), m_shaderId(shaderid), m_textures(textures) {};
		SCM::TextureMap m_textures; //textures plus name
		ipengine::ipid m_shaderId;
		ipengine::ipid m_materialId;
	};

	class TextureFile
	{
	public:
		TextureFile() = default;
		TextureFile(ipengine::ipid id, std::string path, bool isCube = false) : m_textureId(id), m_path(path), m_isCube(isCube){};
		ipengine::ipid m_textureId;
		std::string m_path;
		bool m_isCube;
	};

	class TextureData
	{
	public:
		TextureData() = default;
		TextureData(ipengine::ipid fileid, glm::vec2 offset = glm::vec2( 0,0 ), glm::vec2 size= glm::vec2(0,0)) :m_texturefileId(fileid), m_offset(offset), m_size(size) {}
		ipengine::ipid m_texturefileId;
		glm::vec2 m_offset;
		glm::vec2 m_size;
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
		ipengine::ipid m_meshId;
		bool m_dynamic;
		bool m_dirty;

	};
	
	class MeshedObject
	{
	public:
		MeshedObject(std::vector<MeshData*>& mdata, ipengine::ipid id) :m_meshes(mdata), m_meshObjectId(id) {}
		ipengine::ipid m_meshObjectId;
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
		Entity() :
			m_entityId(-1),
			m_parent(nullptr),
			isBoundingBox(true),
			isActive(false)
		{}
		Entity(const Entity& other):
			m_transformData(other.m_transformData),
			m_parent(other.m_parent),
			m_name(other.m_name),
			m_boundingData(other.m_boundingData),
			isBoundingBox(other.isBoundingBox),
			isActive(other.isActive)
		{
			m_entityId = -1;
		}
		Entity(
			ipengine::ipid id,
			Transform& transform,
			BoundingData& boundingdata,
			bool boundingbox,
			bool active):
				m_entityId(id),
				m_parent(nullptr),
				isBoundingBox(boundingbox),
				isActive(active),
				m_transformData(transform),
				m_boundingData(boundingdata)
		{}

		//virtual ~Entity() {};

		Transform m_transformData;
		Entity* m_parent;
		ipengine::ipid m_entityId;
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
		ThreeDimEntity(
			ipengine::ipid id,
			Transform& transform,
			BoundingData& boundingdata,
			bool boundingbox,
			bool active,
			MeshedObject* meshes):
				Entity(id, transform, boundingdata, boundingbox, active),
				m_mesheObjects(meshes)
		{
		}
		MeshedObject* m_mesheObjects;
		virtual void swap()
		{
			m_transformData.swap();
			m_mesheObjects->swap();
		}

		void generateBoundingBox()
		{
			glm::vec3 min(std::numeric_limits<float>::max());
			glm::vec3 max(std::numeric_limits<float>::lowest());

			for (auto& m : m_mesheObjects->m_meshes)
			{
				for (auto& v : m->m_vertices.getData())
				{
					min.x = glm::min(min.x, v.m_position.x);
					min.y = glm::min(min.y, v.m_position.y);
					min.z = glm::min(min.z, v.m_position.z);

					max.x = glm::max(max.x, v.m_position.x);
					max.y = glm::max(max.y, v.m_position.y);
					max.z = glm::max(max.z, v.m_position.z);
				}
			}

			SCM::BoundingBox bb;
			bb.m_center = min + (max - min) * 0.5f;
			bb.m_rotation = glm::quat();
			bb.m_size = glm::vec3(max.x - min.x, max.y - min.y, max.z - min.z);
			this->isBoundingBox = true;
			this->m_boundingData.box = bb;
		}

		void generateBoundingSphere()
		{
			if (m_mesheObjects->m_meshes.size() == 0)
				return;
			//Ritter's algorithm for minial bounding sphere approximation
			SCM::BoundingSphere bs;
			bs.m_center = glm::vec3(0.0f);
			bs.m_radius = 0.0f;

			glm::vec3 x = m_mesheObjects->m_meshes[0]->m_vertices.getData()[0].m_position;
			glm::vec3 y = x;
			for (auto& m : m_mesheObjects->m_meshes)
			{
				for (auto& v : m->m_vertices.getData())
				{
					if (glm::length(v.m_position - x) > glm::length(y - x))
						y = v.m_position;
				}
			}
			glm::vec3 z = x;
			for (auto& m : m_mesheObjects->m_meshes)
			{
				for (auto& v : m->m_vertices.getData())
				{
					if (glm::length(v.m_position - y) > glm::length(z - y))
						z = v.m_position;
				}
			}

			bs.m_center = y + (z - y) * 0.5f;
			bs.m_radius = glm::length(z - y) * 0.5f;

			for (auto& m : m_mesheObjects->m_meshes)
			{
				for (auto& v : m->m_vertices.getData())
				{
					if (glm::length(v.m_position - bs.m_center) > bs.m_radius)
					{
						glm::vec3 ctp = v.m_position - bs.m_center;
						glm::vec3 nd1 = bs.m_center + (-glm::normalize(ctp) * bs.m_radius);
						bs.m_radius = glm::length(v.m_position - nd1) * 0.5f;
						bs.m_center = nd1 + (v.m_position - nd1) * 0.5f;
					}
				}
			}

			this->isBoundingBox = false;
			this->m_boundingData.sphere = bs;
		}
	};

	//lights

	class DirectionalLight : public Entity
	{
	public:
		DirectionalLight(
			//Entity params
			ipengine::ipid id,
			Transform& transform,
			BoundingData& boundingdata,
			bool boundingbox,
			bool active,
			//Light params
			const glm::vec3& color) :
				Entity(id, transform, boundingdata, boundingbox, active),
				m_color(color)
		{
		}

		glm::vec3 getVSDirection(const glm::mat4& viewmat)
		{
			return glm::mat3(viewmat) * (-m_transformData.getData()->m_localZ);
		}

		glm::vec3 getDirection()
		{
			return -m_transformData.getData()->m_localZ;
		}

		glm::vec3 getVSPosition(const glm::mat4& viewmat)
		{
			return glm::vec3(viewmat * glm::vec4(m_transformData.getData()->m_location, 1.0f));
		}

		glm::vec3 getPosition()
		{
			return m_transformData.getData()->m_location;
		}

		glm::vec3 m_color;
	};

	class PointLight : public Entity
	{
	public:
		PointLight(
			//Entity params
			ipengine::ipid id,
			Transform& transform,
			BoundingData& boundingdata,
			bool boundingbox,
			bool active,
			//Light params
			const glm::vec3& color,
			float maxRange) :
				Entity(id, transform, boundingdata, boundingbox, active),
				m_color(color),
				m_range(maxRange)
		{}

		glm::vec3 getVSPosition(const glm::mat4& viewmat)
		{
			return glm::vec3(viewmat * glm::vec4(m_transformData.getData()->m_location, 1.0f));
		}

		glm::vec3 getPosition()
		{
			return m_transformData.getData()->m_location;
		}

		glm::vec3 m_color;
		float m_range;
	};

	class SpotLight : public Entity
	{
	public:
		SpotLight(
			//Entity params
			ipengine::ipid id,
			Transform& transform,
			BoundingData& boundingdata,
			bool boundingbox,
			bool active,
			//Light params
			const glm::vec3& color,
			float maxRange,
			float innerConeAngle,
			float outerConeAngle) :
				Entity(id, transform, boundingdata, boundingbox, active),
				m_color(color),
				m_range(maxRange),
				m_innerConeAngle(innerConeAngle),
				m_outerConeAngle(outerConeAngle)
		{}

		glm::vec3 getVSDirection(const glm::mat4& viewmat)
		{
			return glm::mat3(viewmat) * (-m_transformData.getData()->m_localZ);
		}

		glm::vec3 getDirection()
		{
			return -m_transformData.getData()->m_localZ;
		}

		glm::vec3 getPosition()
		{
			return m_transformData.getData()->m_location;
		}

		glm::vec3 getVSPosition(const glm::mat4& viewmat)
		{
			return glm::vec3(viewmat * glm::vec4(m_transformData.getData()->m_location, 1.0f));
		}

		glm::vec3 m_color;
		float m_range;
		float m_innerConeAngle;
		float m_outerConeAngle;
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

		virtual std::unordered_map<ipengine::ipid, ThreeDimEntity*>& getThreeDimEntities()
		{
			return threedimentities;
		}

		virtual std::unordered_map<ipengine::ipid, DirectionalLight*>& getDirLights()
		{
			return dirLights;
		}

		virtual std::unordered_map<ipengine::ipid, PointLight*>& getPointLights()
		{
			return pointLights;
		}

		virtual std::unordered_map<ipengine::ipid, SpotLight*>& getSpotLights()
		{
			return spotLights;
		}

		virtual std::vector<ShaderData>& getShaders()
		{
			return shaders;
		}

		virtual std::vector<MaterialData>& getMaterials()
		{
			return materials;
		}

		virtual std::vector<TextureFile>& getTextures()
		{
			return texturefiles;
		}

		virtual std::vector<MeshData>& getMeshes()
		{
			return meshes;
		}

		virtual std::vector<MeshedObject>& getMeshedObjects()
		{
			return meshedobjects;
		}

		virtual TextureFile* getTextureById(ipengine::ipid id)
		{
			auto itF = std::find_if(texturefiles.begin(), texturefiles.end(), [id](TextureFile& a)->bool {return a.m_textureId == id; });
			if (itF != texturefiles.end())
			{
				return &*itF;
			}
			else
				return nullptr;
		}
		virtual MaterialData* getMaterialById(ipengine::ipid id)
		{ 
			auto itF = std::find_if(materials.begin(), materials.end(), [id](MaterialData& a)->bool {return a.m_materialId == id; });
			if (itF != materials.end())
			{
				return &*itF;
			}
			else
				return nullptr; 
		};
		virtual ShaderData* getShaderById(ipengine::ipid id)
		{ 
			auto itF = std::find_if(shaders.begin(), shaders.end(), [id](ShaderData& a)->bool {return a.m_shaderId == id; });
			if (itF != shaders.end())
			{
				return &*itF;
			}
			else
				return nullptr;
		};
		virtual Entity* getEntityById(ipengine::ipid id)
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
		virtual MeshData* getMeshById(ipengine::ipid id)
		{ 
			auto itF = std::find_if(meshes.begin(), meshes.end(), [id](MeshData& a)->bool {return a.m_meshId == id; });
			if (itF != meshes.end())
			{
				return &*itF;
			}
			else
				return nullptr;
		}
		virtual MeshedObject* getMeshedObjectById(ipengine::ipid id)
		{ 
			auto itF = std::find_if(meshedobjects.begin(), meshedobjects.end(), [id](MeshedObject& a)->bool {return a.m_meshObjectId == id; });
			if (itF != meshedobjects.end())
			{
				return &*itF;
			}
			else
				return nullptr;
		}
		virtual ipengine::ipid addMeshFromFile(std::string path, std::string format, std::vector<ipengine::ipid> mats) = 0;
		virtual ipengine::ipid getDefaultShaderId() = 0;
		virtual ipengine::ipid getDefaultTextureId() = 0;

		virtual bool setEntityParent(ipengine::ipid child, ipengine::ipid parent)
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
		std::unordered_map<ipengine::ipid, ThreeDimEntity*> threedimentities;
		std::unordered_map<ipengine::ipid, DirectionalLight*> dirLights;
		std::unordered_map<ipengine::ipid, PointLight*> pointLights;
		std::unordered_map<ipengine::ipid, SpotLight*> spotLights;
		std::vector<ShaderData> shaders;
		std::vector<MaterialData> materials;
		std::vector<TextureFile> texturefiles;
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