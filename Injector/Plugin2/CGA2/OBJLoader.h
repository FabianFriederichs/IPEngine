#ifndef _OBJ_LOADER_
#define _OBJ_LOADER_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cctype>
#include <unordered_map>

//------------------------------ istream string helper ----------------------------------------

class istreamhelper
{
public:
	static bool peekString(std::istream& stream, std::string& out);
	static bool consumeString(std::istream& stream);
	template <typename T>
	static bool peek(std::istream& stream, T& out)
	{
		try
		{
			if (stream.eof())
			{
				return false;
			}
			auto spos = stream.tellg();
			if (stream >> out)
			{
				stream.seekg(spos);
				return true;
			}
			else
			{
				if (!stream.eof())
					stream.seekg(spos);
				return false;
			}
		}
		catch (std::exception ex)
		{
			throw ex;
		}
	}
};

//------------------------------ Data Structures to hold the result ---------------------------
class OBJException : public std::logic_error
{
public:
	OBJException() : std::logic_error("unkown exception")
	{
	}
	OBJException(const char* msg) : std::logic_error(msg)
	{
	}
};

struct Vertex
{
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	glm::vec3 tangent;
};

typedef GLuint Index;

class OBJMesh
{
public:
	OBJMesh() : 
		hasPositions(false),
		hasUVs(false),
		hasNormals(false),
		hasTangents(false)
	{}
	OBJMesh(const OBJMesh& other) :
		name(other.name),
		hasPositions(other.hasPositions),
		hasUVs(other.hasUVs),
		hasNormals(other.hasNormals),
		hasTangents(other.hasTangents),
		vertices(other.vertices),
		indices(other.indices)
	{}
	OBJMesh(OBJMesh&& other) :
		name(std::move(other.name)),
		hasPositions(other.hasPositions),
		hasUVs(other.hasUVs),
		hasNormals(other.hasNormals),
		hasTangents(other.hasTangents),
		vertices(std::move(other.vertices)),
		indices(std::move(other.indices))
	{}
	~OBJMesh() {}

	OBJMesh& operator=(const OBJMesh& other)
	{
		if (this == &other)
			return *this;
		this->name = other.name;
		this->hasPositions = other.hasPositions;
		this->hasUVs = other.hasUVs;
		this->hasNormals = other.hasNormals;
		this->hasTangents = other.hasTangents;
		this->vertices = other.vertices;
		this->indices = other.indices;

		return *this;
	}
	OBJMesh& operator=(OBJMesh&& other)
	{
		if (this == &other)
			return *this;
		this->name = std::move(other.name);
		this->hasPositions = other.hasPositions;
		this->hasUVs = other.hasUVs;
		this->hasNormals = other.hasNormals;
		this->hasTangents = other.hasTangents;
		this->vertices = std::move(other.vertices);
		this->indices = std::move(other.indices);

		return *this;
	}

	bool hasPositions;
	bool hasUVs;
	bool hasNormals;
	bool hasTangents;

	std::string name;

	std::vector<Vertex> vertices;
	std::vector<Index> indices;
};

class OBJObject
{
public:
	OBJObject() {}
	OBJObject(const OBJObject& other) :
		name(other.name),
		meshes(other.meshes)
	{}
	OBJObject(OBJObject&& other) :
		name(std::move(other.name)),
		meshes(std::move(other.meshes))
	{}
	~OBJObject() {}

	OBJObject& operator=(const OBJObject& other)
	{
		if (this == &other)
			return *this;

		this->name = other.name;
		this->meshes = other.meshes;

		return *this;
	}
	OBJObject& operator=(OBJObject&& other)
	{
		if (this == &other)
			return *this;

		this->name = std::move(other.name);
		this->meshes = std::move(other.meshes);

		return *this;
	}

	std::string name;
	std::vector<OBJMesh> meshes;
};

class OBJResult
{
public:
	OBJResult() {}
	OBJResult(const OBJResult& other) :
		objname(other.objname),
		objects(other.objects)
	{}
	OBJResult(OBJResult&& other) :
		objname(std::move(other.objname)),
		objects(std::move(other.objects))
	{}
	~OBJResult() {}

	OBJResult& operator=(const OBJResult& other)
	{
		if (this == &other)
			return *this;

		this->objname = other.objname;
		this->objects = other.objects;

		return *this;
	}
	OBJResult& operator=(OBJResult&& other)
	{
		if (this == &other)
			return *this;

		this->objname = std::move(other.objname);
		this->objects = std::move(other.objects);

		return *this;
	}

	std::string objname;
	std::vector<OBJObject> objects;
};

class OBJLoader
{
private:
	OBJLoader();
	~OBJLoader();

public:
	static OBJResult loadOBJ(const std::string& objpath, bool calcnormals = false, bool calctangents = false);

	class DataCache
	{
	public:
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
	};

	class VertexDef
	{
	public:
		//if -1: undefined
		Index p_idx = 0;
		bool p_defined = false;
		Index uv_idx = 0;
		bool uv_defined = false;
		Index n_idx = 0;
		bool n_defined = false;

		class hash
		{
		public:
			std::hash<int> h;
			size_t operator()(const VertexDef& vd) const
			{
				size_t seed = 0;				
				seed ^= h(vd.p_idx) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				seed ^= h(vd.uv_idx) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				seed ^= h(vd.n_idx) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				return seed;
			}
			//seed ^= hash_value(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		};

		class equal_to
		{
		public:
			bool operator()(const VertexDef& vd1, const VertexDef& vd2) const
			{
				return (vd1.p_idx == vd2.p_idx && vd1.uv_idx == vd2.uv_idx && vd1.n_idx == vd2.n_idx);
			}
		};
		
	};

	class Face
	{
	public:
		std::vector<VertexDef> verts;
	};

private:
	//parsing helpers
	//o flag
	static OBJObject parseObject(DataCache& cache, std::ifstream& stream, bool calcnormals = false, bool calctangents = false);
	//v per o
	static glm::vec3 parsePosition(std::ifstream& stream);
	//vn per o
	static glm::vec3 parseNormal(std::ifstream& stream);
	//vt per o
	static glm::vec2 parseUV(std::ifstream& stream);
	//=> create raw v, vn, vt cache

	//parse f flags and call parseFace
	static OBJMesh parseMesh(DataCache& cache ,std::ifstream& stream, bool calcnormals = false, bool calctangents = false);

	//parse face and generate vertices and indices for the mesh
	static Face parseFace(std::ifstream& stream);

	//create Vertex from "v/vt/vn" strings
	static VertexDef parseVertex(const std::string& vstring);

	//fill mesh
	static void fillMesh(OBJMesh& mesh, DataCache& cache, std::vector<VertexDef> vdefs, std::vector<Index>& indices);



public:
	//post processing
	static void recalculateNormals(OBJMesh& mesh);
	static void recalculateTangents(OBJMesh& mesh);
	static void reverseWinding(OBJMesh& mesh);
};



#endif
