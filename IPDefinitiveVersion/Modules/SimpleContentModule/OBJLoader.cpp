#include "OBJLoader.h"



OBJLoader::OBJLoader()
{}


OBJLoader::~OBJLoader()
{}

OBJResult OBJLoader::loadOBJ(const std::string & objpath, bool calcnormals, bool calctangents)
{
	OBJResult result;
	try
	{
		std::ifstream stream(objpath, std::ios_base::in | std::ios_base::binary);
		stream.exceptions(std::ifstream::badbit);
		if (!stream.is_open())
			throw std::logic_error("OBJ file not found.");
		std::string command = "";
		DataCache cache;
		while (istreamhelper::peekString(stream, command))
		{
			if (command == "o")
			{
				result.objects.push_back(parseObject(cache, stream, calcnormals, calctangents));
			}
			else if (command == "v" || command == "vt" || command == "vn" || command == "g" || command == "f") //no object. parse whole obj file into one object
			{
				result.objects.push_back(parseObject(cache, stream, calcnormals, calctangents));
			}
			else
			{
				stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
		}
		result.objname = objpath;
		return result;
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error: Loading OBJ failed: " << ex.what() << "\n";
		throw ex;
	}
}

OBJObject OBJLoader::parseObject(DataCache& cache, std::ifstream & stream, bool calcnormals, bool calctangents)
{
	try
	{
		OBJObject object;
		std::string command;

		//get object name
		if (!istreamhelper::peekString(stream, command))
			throw OBJException("Error parsing object.");

		if (command == "o")
		{
			if (!(stream >> command))
				throw OBJException("Error parsing object name.");
			if (!std::getline(stream, object.name))
				throw OBJException("Error parsing object name.");
		}
		else
		{
			object.name = "UNNAMED";
		}

		while (istreamhelper::peekString(stream, command))
		{
			//Fill cache
			if (command == "v")			//position
			{
				cache.positions.push_back(parsePosition(stream));
			}
			else if (command == "vt")	//uv
			{
				cache.uvs.push_back(parseUV(stream));
			}
			else if (command == "vn")	//normal
			{
				cache.normals.push_back(parseNormal(stream));
			}

			//meshes, groups and faces
			else if (command == "g" || command == "f") //grouped or ungrouped mesh
			{
				object.meshes.push_back(parseMesh(cache, stream, calcnormals, calctangents));
			}
			//stop condition
			else if (command == "o") //next object found
			{
				return object;
			}

			//ignore everything else
			else
			{
				stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
		} //eof reached. model should be complete
		if (stream.eof())
		{
			return object;
		}
		else
		{
			throw OBJException("Error parsing object. Read failed before end of file.");
		}
	}
	catch (const std::exception& ex)
	{
		throw ex;
	}
}

glm::vec3 OBJLoader::parsePosition(std::ifstream & stream)
{
	try
	{
		std::string command;
		double x, y, z;
		if (!(stream >> command >> x >> y >> z) || command != "v")
		{
			throw OBJException("Error parsing v command.");
		}
		return glm::vec3(x, y, z);
	}
	catch (const std::exception& ex)
	{
		throw ex;
	}
}

glm::vec3 OBJLoader::parseNormal(std::ifstream & stream)
{
	try
	{
		std::string command;
		double x, y, z;
		if (!(stream >> command >> x >> y >> z) || command != "vn")
		{
			throw OBJException("Error parsing vn command.");
		}
		return glm::vec3(x, y, z);
	}
	catch (const std::exception& ex)
	{
		throw ex;
	}
}

glm::vec2 OBJLoader::parseUV(std::ifstream & stream)
{
	try
	{
		std::string command;
		double u, v;
		if (!(stream >> command >> u >> v) || command != "vt")
		{
			throw OBJException("Error parsing vt command.");
		}
		return glm::vec2(u, v);
	}
	catch (const std::exception& ex)
	{
		throw ex;
	}
}

OBJMesh OBJLoader::parseMesh(DataCache & cache, std::ifstream & stream, bool calcnormals, bool calctangents)
{
	try
	{
		OBJMesh mesh;
		std::unordered_map<VertexDef, size_t, VertexDef::hash, VertexDef::equal_to> meshvertset; //collect distinct vertices

																								 //later create actual vertices out of these and put them into the mesh
		std::vector<VertexDef> meshverts; //for tracking order of insertion		
		std::vector<Index> meshindices;	//Vertex index of one of the Vertex defs above

		std::string command;
		if (!(istreamhelper::peekString(stream, command)))
		{
			throw OBJException("Error parsing mesh.");
		}

		if (command == "g") //if we have a grouped mesh extract its name first
		{
			if (!(stream >> command))
			{
				throw OBJException("Error parsing mesh.");
			}

			if (!std::getline(stream, mesh.name))
			{
				throw OBJException("Error parsing mesh name.");
			}
		}
		else
		{
			mesh.name = "UNGROUPED";
		}

		//now process faces
		while (istreamhelper::peekString(stream, command))
		{
			if (command == "f") //yay we found a face
			{
				Face face = parseFace(stream);
				//process face data and build mesh
				for (int i = 0; i < 3; i++)
				{
					//add Vertexdefs and indices
					auto it = meshvertset.find(face.verts[i]);
					if (it != meshvertset.end())	//if Vertex def exists already, just get the index and push it onto index array
					{
						meshindices.push_back(static_cast<Index>(it->second));
					}
					else //if not, push a index pointing to the last pushed Vertex def, push Vertex def and insert it into the set
					{
						meshindices.push_back(static_cast<Index>(meshverts.size()));
						meshverts.push_back(face.verts[i]);
						meshvertset.insert(std::make_pair(face.verts[i], meshverts.size() - 1));
					}
				}
			}
			else if (command == "g" || command == "o") //found next mesh group
			{
				fillMesh(mesh, cache, meshverts, meshindices);
				if (calcnormals)
					recalculateNormals(mesh);
				if (calctangents)
					recalculateTangents(mesh);
				return mesh;
			}
			//new vertex data => no objects or mesh groups are present. simply put the new data into cache
			else if (command == "v")			//position
			{
				cache.positions.push_back(parsePosition(stream));
			}
			else if (command == "vt")	//uv
			{
				cache.uvs.push_back(parseUV(stream));
			}
			else if (command == "vn")	//normal
			{
				cache.normals.push_back(parseNormal(stream));
			}
			else
			{
				stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
		}
		if (stream.eof())
		{
			fillMesh(mesh, cache, meshverts, meshindices);
			if (calcnormals)
				recalculateNormals(mesh);
			if (calctangents)
				recalculateTangents(mesh);
			return mesh;
		}
		else
		{
			throw OBJException("Error parsing mesh. Read failed before end of file.");
		}
	}
	catch (const std::exception& ex)
	{
		throw ex;
	}
}

OBJLoader::Face OBJLoader::parseFace(std::ifstream & stream)
{
	try
	{
		std::string command;
		Face face;
		if ((stream >> command) && command == "f")
		{
			std::string vstr1, vstr2, vstr3;
			if (stream >> vstr1 >> vstr2 >> vstr3)
			{
				face.verts.push_back(parseVertex(vstr1));
				face.verts.push_back(parseVertex(vstr2));
				face.verts.push_back(parseVertex(vstr3));
				return face;
			}
			else
			{
				throw OBJException("Error parsing face");
			}
		}
		else
		{
			throw OBJException("Error parsing face");
		}
	}
	catch (const std::exception& ex)
	{
		throw ex;
	}
}

OBJLoader::VertexDef OBJLoader::parseVertex(const std::string& vstring)
{
	try
	{
		//buffer for v, vt, vn index
		std::string att[3];

		int attct = 0;

		for (size_t i = 0; i < vstring.length(); i++)
		{
			if (isdigit(vstring[i]))
			{
				att[attct] += vstring[i];
			}
			else if (vstring[i] == '/')
			{
				attct++;
			}
			else
			{
				throw OBJException("Error parsing Vertex.");
			}
		}

		VertexDef vert;
		vert.p_idx = (att[0].length() > 0 ? std::stoi(att[0]) - 1 : 0);
		vert.p_defined = (att[0].length() > 0 ? true : false);
		vert.uv_idx = (att[1].length() > 0 ? std::stoi(att[1]) - 1 : 0);
		vert.uv_defined = (att[1].length() > 0 ? true : false);
		vert.n_idx = (att[2].length() > 0 ? std::stoi(att[2]) - 1 : 0);
		vert.n_defined = (att[2].length() > 0 ? true : false);
		return vert;
	}
	catch (const std::exception& ex)
	{
		throw ex;
	}
}

void OBJLoader::fillMesh(OBJMesh & mesh, DataCache & cache, std::vector<VertexDef> vdefs, std::vector<Index>& indices)
{
	try
	{
		//assemble the mesh from the collected indices
		//create Vertex from cache data
		bool hasverts = true;
		bool hasuvs = true;
		bool hasnormals = true;
		for (size_t i = 0; i < vdefs.size(); i++)
		{
			Vertex vert;

			if (vdefs[i].p_defined)
			{
				if (vdefs[i].p_idx < static_cast<Index>(cache.positions.size()))
					vert.position = cache.positions[vdefs[i].p_idx];
				else
					throw OBJException("Missing position in object definition");
			}
			else
			{
				hasverts = false;
			}

			if (vdefs[i].uv_defined)
			{
				if (vdefs[i].uv_idx < static_cast<Index>(cache.uvs.size()))
					vert.uv = cache.uvs[vdefs[i].uv_idx];
				else
					throw OBJException("Missing texture coordinate in object definition");
			}
			else
			{
				hasuvs = false;
			}

			if (vdefs[i].n_defined)
			{
				if (vdefs[i].n_idx < static_cast<Index>(cache.normals.size()))
					vert.normal = cache.normals[vdefs[i].n_idx];
				else
					throw OBJException("Missing normal in object definition");
			}
			else
			{
				hasnormals = false;
			}

			mesh.vertices.push_back(vert);
		}
		mesh.indices = std::move(indices);
		mesh.hasPositions = hasverts;
		mesh.hasUVs = hasuvs;
		mesh.hasNormals = hasnormals;
	}
	catch (const std::exception& ex)
	{
		throw ex;
	}
}

void OBJLoader::recalculateNormals(OBJMesh & mesh)
{
	try
	{
		for (size_t i = 0; i < mesh.vertices.size(); i++) //initialize all Vertex normals with nullvectors
		{
			mesh.vertices[i].normal = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			glm::vec3 v1 = mesh.vertices[mesh.indices[i]].position;
			glm::vec3 v2 = mesh.vertices[mesh.indices[i + 1]].position;
			glm::vec3 v3 = mesh.vertices[mesh.indices[i + 2]].position;

			//counter clockwise winding
			glm::vec3 edge1 = v2 - v1;
			glm::vec3 edge2 = v3 - v1;

			glm::vec3 normal = glm::cross(edge1, edge2);

			//for each Vertex all corresponing normals are added. The result is a non unit length vector wich is the average direction of all assigned normals.
			mesh.vertices[mesh.indices[i]].normal += normal;
			mesh.vertices[mesh.indices[i + 1]].normal += normal;
			mesh.vertices[mesh.indices[i + 2]].normal += normal;
		}

		for (size_t i = 0; i < mesh.vertices.size(); i++)	//normalize all normals calculated in the previous step
		{
			mesh.vertices[i].normal = glm::normalize(mesh.vertices[i].normal);
		}

		mesh.hasNormals = true;
	}
	catch (const std::exception& ex)
	{
		throw ex;
	}
}

void OBJLoader::recalculateTangents(OBJMesh & mesh)
{
	try
	{
		if (mesh.hasUVs)
		{
			//initialize tangents and bitangents with nullvecs
			for (size_t i = 0; i < mesh.vertices.size(); i++)
			{
				mesh.vertices[i].tangent = glm::vec3(0.0f, 0.0f, 0.0f);
			}

			float det;
			glm::vec3 tangent;
			glm::vec3 normal;

			//calculate and average tangents and bitangents just as we did when calculating the normals
			for (size_t i = 0; i < mesh.indices.size(); i += 3)
			{
				//3 vertices of a triangle
				glm::vec3 v1 = mesh.vertices[mesh.indices[i]].position;
				glm::vec3 v2 = mesh.vertices[mesh.indices[i + 1]].position;
				glm::vec3 v3 = mesh.vertices[mesh.indices[i + 2]].position;

				//uvs
				glm::vec2 uv1 = mesh.vertices[mesh.indices[i]].uv;
				glm::vec2 uv2 = mesh.vertices[mesh.indices[i + 1]].uv;
				glm::vec2 uv3 = mesh.vertices[mesh.indices[i + 2]].uv;

				//calculate edges in counter clockwise winding order
				glm::vec3 edge1 = v2 - v1;
				glm::vec3 edge2 = v3 - v1;

				//deltaus and deltavs
				glm::vec2 duv1 = uv2 - uv1;
				glm::vec2 duv2 = uv3 - uv1;

				det = duv1.x * duv2.y - duv2.x * duv1.y;

				if (fabs(det) < 1e-6f)		//if delta stuff is close to nothing ignore it
				{
					tangent = glm::vec3(1.0f, 0.0f, 0.0f);
				}
				else
				{
					det = 1.0f / det;

					tangent.x = det * (duv2.y * edge1.x - duv1.y * edge2.x);
					tangent.y = det * (duv2.y * edge1.y - duv1.y * edge2.y);
					tangent.z = det * (duv2.y * edge1.z - duv1.y * edge2.z);
				}

				mesh.vertices[mesh.indices[i]].tangent += tangent;
				mesh.vertices[mesh.indices[i + 1]].tangent += tangent;
				mesh.vertices[mesh.indices[i + 2]].tangent += tangent;
			}

			//orthogonalize and normalize tangents
			for (size_t i = 0; i < mesh.vertices.size(); i++)
			{
				//normalize the stuff from before
				mesh.vertices[i].normal = glm::normalize(mesh.vertices[i].normal);
				mesh.vertices[i].tangent = glm::normalize(mesh.vertices[i].tangent);

				//gram schmidt reorthogonalize normal-tangent system
				mesh.vertices[i].tangent = glm::normalize(mesh.vertices[i].tangent - (glm::dot(mesh.vertices[i].normal, mesh.vertices[i].tangent) * mesh.vertices[i].normal));
			}
			mesh.hasTangents = true;
		}
	}
	catch (const std::exception& ex)
	{
		throw ex;
	}
}

void OBJLoader::reverseWinding(OBJMesh & mesh)
{
	try
	{
		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			Index tmp = mesh.indices[i + 1];
			mesh.indices[i + 1] = mesh.indices[i + 2];
			mesh.indices[i + 2] = tmp;
		}
	}
	catch (const std::exception& ex)
	{
		throw ex;
	}
}

SCM::BoundingBox OBJLoader::createBoundingBox(OBJMesh & mesh)
{
	glm::vec3 min(std::numeric_limits<float>::max());
	glm::vec3 max(std::numeric_limits<float>::lowest());
	
	for (auto& v : mesh.vertices)
	{
		min.x = glm::min(min.x, v.position.x);
		min.y = glm::min(min.y, v.position.y);
		min.z = glm::min(min.z, v.position.z);

		max.x = glm::max(max.x, v.position.x);
		max.y = glm::max(max.y, v.position.y);
		max.z = glm::max(max.z, v.position.z);
	}	

	SCM::BoundingBox bb;
	bb.m_center = min + (max - min) * 0.5f;
	bb.m_rotation = glm::quat();
	bb.m_size = glm::vec3(max.x - min.x, max.y - min.y, max.z - min.z);

	return bb;
}

SCM::BoundingBox OBJLoader::createBoundingBox(OBJObject & object)
{
	glm::vec3 min(std::numeric_limits<float>::max());
	glm::vec3 max(std::numeric_limits<float>::lowest());

	for (auto& m : object.meshes)
	{
		for (auto& v : m.vertices)
		{
			min.x = glm::min(min.x, v.position.x);
			min.y = glm::min(min.y, v.position.y);
			min.z = glm::min(min.z, v.position.z);

			max.x = glm::max(max.x, v.position.x);
			max.y = glm::max(max.y, v.position.y);
			max.z = glm::max(max.z, v.position.z);
		}
	}

	SCM::BoundingBox bb;
	bb.m_center = min + (max - min) * 0.5f;
	bb.m_rotation = glm::quat();
	bb.m_size = glm::vec3(max.x - min.x, max.y - min.y, max.z - min.z);

	return bb;
}

SCM::BoundingSphere OBJLoader::createBoundingSphere(OBJMesh & mesh)
{
	//Ritter's algorithm for minial bounding sphere approximation
	SCM::BoundingSphere bs;
	bs.m_center = glm::vec3(0.0f);
	bs.m_radius = 0.0f;
	if (mesh.vertices.size() <= 1)
		return bs;

	glm::vec3 x = mesh.vertices[0].position;
	glm::vec3 y = x;
	for (auto& v : mesh.vertices)
	{
		if (glm::length(v.position - x) > glm::length(y - x))
			y = v.position;
	}
	glm::vec3 z = x;
	for (auto& v : mesh.vertices)
	{
		if (glm::length(v.position - y) > glm::length(z - y))
			z = v.position;
	}

	bs.m_center = y + (z - y) * 0.5f;
	bs.m_radius = glm::length(z - y) * 0.5f;

	for (auto& v : mesh.vertices)
	{
		if (length(v.position - bs.m_center) > bs.m_radius)
		{
			glm::vec3 ctp = v.position - bs.m_center;
			glm::vec3 nd1 = bs.m_center + (-glm::normalize(ctp) * bs.m_radius);
			bs.m_radius = length(v.position - nd1) * 0.5f;
			bs.m_center = nd1 + (v.position - nd1) * 0.5f;
		}
	}

	return bs;
}

SCM::BoundingSphere OBJLoader::createBoundingSphere(OBJObject & object)
{
	//Ritter's algorithm for minial bounding sphere approximation
	SCM::BoundingSphere bs;
	bs.m_center = glm::vec3(0.0f);
	bs.m_radius = 0.0f;
	if (object.meshes.size() == 0)
		return bs;
	
	glm::vec3 x = object.meshes[0].vertices[0].position;
	glm::vec3 y = x;
	for (auto& m : object.meshes)
	{
		for (auto& v : m.vertices)
		{
			if (glm::length(v.position - x) > glm::length(y - x))
				y = v.position;
		}
	}
	glm::vec3 z = x;
	for (auto& m : object.meshes)
	{
		for (auto& v : m.vertices)
		{
			if (glm::length(v.position - y) > glm::length(z - y))
				z = v.position;
		}
	}

	bs.m_center = y + (z - y) * 0.5f;
	bs.m_radius = glm::length(z - y) * 0.5f;

	for (auto& m : object.meshes)
	{
		for (auto& v : m.vertices)
		{
			if (length(v.position - bs.m_center) > bs.m_radius)
			{
				glm::vec3 ctp = v.position - bs.m_center;
				glm::vec3 nd1 = bs.m_center + (-glm::normalize(ctp) * bs.m_radius);
				bs.m_radius = glm::length(v.position - nd1) * 0.5f;
				bs.m_center = nd1 + (v.position - nd1) * 0.5f;
			}
		}
	}

	return bs;
}

void OBJLoader::recenter(OBJMesh & mesh)
{
	auto bb = createBoundingBox(mesh);
	glm::vec3 diff = -bb.m_center;

	for (auto& v : mesh.vertices)
	{
		v.position += diff;
	}
}

void OBJLoader::recenter(OBJObject & object)
{
	auto bb = createBoundingBox(object);
	glm::vec3 diff = -bb.m_center;

	for (auto& m : object.meshes)
	{
		for (auto& v : m.vertices)
		{
			v.position += diff;
		}
	}
}

void OBJLoader::normalize(OBJMesh & mesh)
{
	auto bb = createBoundingBox(mesh);
	float diff = glm::max(bb.m_size.x, glm::max(bb.m_size.y, bb.m_size.z));
	float scale = 2.0f / diff;	
	for (auto& v : mesh.vertices)
	{
		v.position -= bb.m_center;
		v.position *= scale;
		v.position += bb.m_center;
	}	
}

void OBJLoader::normalize(OBJObject & object)
{
	auto bb = createBoundingBox(object);
	float diff = glm::max(bb.m_size.x, glm::max(bb.m_size.y, bb.m_size.z));	
	float scale = 2.0f / diff;
	for (auto& m : object.meshes)
	{
		for (auto& v : m.vertices)
		{
			v.position -= bb.m_center;
			v.position *= scale;
			v.position += bb.m_center;
		}
	}	
}

bool istreamhelper::peekString(std::istream& stream, std::string& out)
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
	catch (const std::exception& ex)
	{
		throw ex;
	}
}

bool istreamhelper::consumeString(std::istream& stream)
{
	try
	{
		std::string black_hole;
		if (stream >> black_hole)
			return true;
		else
			return false;
	}
	catch (const std::exception& ex)
	{
		throw ex;
	}
}
