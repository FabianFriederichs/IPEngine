#include "Utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
std::shared_ptr<Texture2D> GLUtils::loadGLTexture(const std::string& path, bool genMipMaps)
{
	GLsizei width;
	GLsizei height;
	GLsizei channels;
	stbi__vertically_flip_on_load = true;
	unsigned char* image = stbi_load(path.c_str(), &width, &height, &channels, 0);
	//unsigned char* image = SOIL_load_image(path.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);
	GLuint texid = 0;
	if (image == nullptr)
	{
		throw std::logic_error("Texture file coudn't be read.");
	}
	else
	{
		GLint internalformat;
		GLenum format;
		switch (channels)
		{
			case 1:
				internalformat = GL_R8;
				format = GL_RED;
				break;
			case 2:
				internalformat = GL_RG8;
				format = GL_RG;
				break;
			case 3:
				internalformat = GL_RGB8;
				format = GL_RGB;
				break;
			case 4:
				internalformat = GL_RGBA8;
				format = GL_RGBA;
				break;
			default:
				internalformat = GL_RGB8;
				format = GL_RGB;
				break;
		}
		glGenTextures(1, &texid); GLERR
		if (texid == 0)
		{
			stbi_image_free(image);
			throw std::logic_error("OpenGL texture object creation failed.");
		}
		glBindTexture(GL_TEXTURE_2D, texid); GLERR
		glTexImage2D(
			GL_TEXTURE_2D,		
			0,					
			internalformat,			
			width,				
			height,				
			0,					
			format,			
			GL_UNSIGNED_BYTE,	
			image				
		);
		if (checkglerror())
		{
			glDeleteTextures(1, &texid);
			//SOIL_free_image_data(image);
			stbi_image_free(image);
			throw std::logic_error("Error. Could not buffer texture data.");
		}
		if(genMipMaps)
			glGenerateMipmap(GL_TEXTURE_2D); GLERR
		glBindTexture(GL_TEXTURE_2D, 0); GLERR
			stbi_image_free(image);

		//SOIL_free_image_data(image);
	}
	return std::make_shared<Texture2D>(texid);
}

std::shared_ptr<TextureCube> GLUtils::loadGLCubeTexture(const std::string & path_px, const std::string & path_nx, const std::string & path_py, const std::string & path_ny, const std::string & path_pz, const std::string & path_nz, bool genMipMaps)
{
	unsigned char* images[6];
	const std::string* paths[6];

	paths[0] = &path_px;
	paths[1] = &path_nx;
	paths[2] = &path_py;
	paths[3] = &path_ny;
	paths[4] = &path_pz;
	paths[5] = &path_nz;

	stbi_set_flip_vertically_on_load(true);

	GLsizei width;
	GLsizei height;
	GLsizei channels;

	for (int i = 0; i < 6; i++)
	{
		images[i] = stbi_load(paths[i]->c_str(), &width, &height, &channels, 0);

		if (images[i] == nullptr)
		{
			for (int k = 0; k < i; k++)
				stbi_image_free(images[k]);
			throw std::logic_error("Texture file coudn't be read.");
		}

		if (width != height)
		{
			for (int k = 0; k < i; k++)
				stbi_image_free(images[k]);
			throw std::logic_error("Cubemaps must be square.");
		}
	}

	GLint internalformat;
	GLenum format;
	switch (channels)
	{
		case 1:
			internalformat = GL_R8;
			format = GL_RED;
			break;
		case 2:
			internalformat = GL_RG8;
			format = GL_RG;
			break;
		case 3:
			internalformat = GL_RGB8;
			format = GL_RGB;
			break;
		case 4:
			internalformat = GL_RGBA8;
			format = GL_RGBA;
			break;
		default:
			internalformat = GL_RGB8;
			format = GL_RGB;
			break;
	}

	GLuint texid = 0;
	
	glGenTextures(1, &texid); GLERR
	if (texid == 0)
	{
		for (int i = 0; i < 6; i++)
			stbi_image_free(images[i]);
		throw std::logic_error("OpenGL texture object creation failed.");
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, texid); GLERR
	for (int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					 0,
					 internalformat,
					 width,
					 height,
					 0,
					 format,
					 GL_UNSIGNED_BYTE,
					 images[i]
		);
	}
	if (checkglerror())
	{
		glDeleteTextures(1, &texid);
		for (int i = 0; i < 6; i++)
			stbi_image_free(images[i]);
		throw std::logic_error("Error. Could not buffer texture data.");
	}

	if (genMipMaps)
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP); GLERR

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0); GLERR

	for (int i = 0; i < 6; i++)
		stbi_image_free(images[i]);
	
	return std::make_shared<TextureCube>(texid);
}

std::shared_ptr<Texture2D> GLUtils::loadGLTextureHDR(const std::string & path, bool genMipMaps, bool halfprecision)
{
	GLsizei width;
	GLsizei height;
	GLsizei channels;
	stbi_set_flip_vertically_on_load(true);
	float* image = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
	GLuint texid = 0;
	if (image == nullptr)
	{
		throw std::logic_error("Texture file coudn't be read.");
	}
	else
	{
		GLint internalformat;
		GLenum format;
		switch (channels)
		{
			case 1:
				internalformat = halfprecision ? GL_R16F : GL_R32F;
				format = GL_RED;
				break;
			case 2:
				internalformat = halfprecision ? GL_RG16F : GL_RG32F;
				format = GL_RG;
				break;
			case 3:
				internalformat = halfprecision ? GL_RGB16F : GL_RGB32F;
				format = GL_RGB;
				break;
			case 4:
				internalformat = halfprecision ? GL_RGBA16F : GL_RGBA32F;
				format = GL_RGBA;
				break;
			default:
				internalformat = halfprecision ? GL_RGB16F : GL_RGB32F;
				format = GL_RGB;
				break;
		}
		glGenTextures(1, &texid); GLERR
		if (texid == 0)
		{
			stbi_image_free(image);
			throw std::logic_error("OpenGL texture object creation failed.");
		}
		glBindTexture(GL_TEXTURE_2D, texid); GLERR
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			internalformat,
			width,
			height,
			0,
			format,
			GL_FLOAT,
			image
		);
		if (checkglerror())
		{
			glDeleteTextures(1, &texid);
			stbi_image_free(image);
			throw std::logic_error("Error. Could not buffer texture data.");
		}
		if (genMipMaps)
			glGenerateMipmap(GL_TEXTURE_2D); GLERR
		glBindTexture(GL_TEXTURE_2D, 0); GLERR
		stbi_image_free(image);
	}
	return std::make_shared<Texture2D>(texid);
}

std::shared_ptr<TextureCube> GLUtils::loadGLCubeTextureHDR(const std::string & path_px, const std::string & path_nx, const std::string & path_py, const std::string & path_ny, const std::string & path_pz, const std::string & path_nz, bool genMipMaps, bool halfprecision)
{
	float* images[6];
	const std::string* paths[6];

	paths[0] = &path_px;
	paths[1] = &path_nx;
	paths[2] = &path_py;
	paths[3] = &path_ny;
	paths[4] = &path_pz;
	paths[5] = &path_nz;

	stbi_set_flip_vertically_on_load(true);

	GLsizei width;
	GLsizei height;
	GLsizei channels;

	for (int i = 0; i < 6; i++)
	{
		images[i] = stbi_loadf(paths[i]->c_str(), &width, &height, &channels, 0);

		if (images[i] == nullptr)
		{
			for (int k = 0; k < i; k++)
				stbi_image_free(images[k]);
			throw std::logic_error("Texture file coudn't be read.");
		}

		if (width != height)
		{
			for (int k = 0; k < i; k++)
				stbi_image_free(images[k]);
			throw std::logic_error("Cubemaps must be square.");
		}
	}

	GLint internalformat;
	GLenum format;
	switch (channels)
	{
		case 1:
			internalformat = halfprecision ? GL_R16F : GL_R32F;
			format = GL_RED;
			break;
		case 2:
			internalformat = halfprecision ? GL_RG16F : GL_RG32F;
			format = GL_RG;
			break;
		case 3:
			internalformat = halfprecision ? GL_RGB16F : GL_RGB32F;
			format = GL_RGB;
			break;
		case 4:
			internalformat = halfprecision ? GL_RGBA16F : GL_RGBA32F;
			format = GL_RGBA;
			break;
		default:
			internalformat = halfprecision ? GL_RGB16F : GL_RGB32F;
			format = GL_RGB;
			break;
	}

	GLuint texid = 0;

	glGenTextures(1, &texid); GLERR
	if (texid == 0)
	{
		for (int i = 0; i < 6; i++)
			stbi_image_free(images[i]);
		throw std::logic_error("OpenGL texture object creation failed.");
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, texid); GLERR
	for (int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						0,
						internalformat,
						width,
						height,
						0,
						format,
						GL_FLOAT,
						images[i]
		);
	}
	if (checkglerror())
	{
		glDeleteTextures(1, &texid);
		for (int i = 0; i < 6; i++)
			stbi_image_free(images[i]);
		throw std::logic_error("Error. Could not buffer texture data.");
	}

	if (genMipMaps)
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP); GLERR

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0); GLERR

	for (int i = 0; i < 6; i++)
		stbi_image_free(images[i]);

	return std::make_shared<TextureCube>(texid);
}

std::shared_ptr<VAO> GLUtils::createVAO(const SCM::MeshData& mesh)
{
	GLuint vao;
	glGenVertexArrays(1, &vao); GLERR
	if (vao == 0)
	{
		throw std::logic_error("VAO could not be created.");
	}
	GLuint vbo;
	glGenBuffers(1, &vbo);
	if (vbo == 0)
	{
		glDeleteVertexArrays(1, &vao);
		throw std::logic_error("VBO could not be created.");
	}
	GLuint ibo;
	glGenBuffers(1, &ibo);
	if (ibo == 0)
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		throw std::logic_error("IBO could not be created.");
	}
	auto& vertices = mesh.m_vertices.getData();
	glBindVertexArray(vao);	GLERR
	glBindBuffer(GL_ARRAY_BUFFER, vbo); GLERR
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); GLERR
	glBufferData(GL_ARRAY_BUFFER, sizeof(SCM::VertexData) * mesh.m_vertices.getData().size(), reinterpret_cast<const GLvoid*>(mesh.m_vertices.getData().data()), GL_STATIC_DRAW); GLERR
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(SCM::index) * mesh.m_indices.size(), reinterpret_cast<const GLvoid*>(mesh.m_indices.data()), GL_STATIC_DRAW); GLERR
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SCM::VertexData), reinterpret_cast<void*>(offsetof(SCM::VertexData, m_position))); GLERR
	glEnableVertexAttribArray(0); GLERR											
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SCM::VertexData), reinterpret_cast<void*>(offsetof(SCM::VertexData, m_uv))); GLERR
	glEnableVertexAttribArray(1); GLERR
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(SCM::VertexData), reinterpret_cast<void*>(offsetof(SCM::VertexData, m_normal))); GLERR
	glEnableVertexAttribArray(2); GLERR
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SCM::VertexData), reinterpret_cast<void*>(offsetof(SCM::VertexData, m_tangent))); GLERR
	glEnableVertexAttribArray(3); GLERR
	glBindBuffer(GL_ARRAY_BUFFER, 0); GLERR
	glBindVertexArray(0); GLERR
	return std::make_shared<VAO>(vbo, ibo, vao, mesh.m_indices.size());
}

std::shared_ptr<VAO> GLUtils::createDynamicVAO(const SCM::MeshData & mesh)
{
	GLuint vao;
	glGenVertexArrays(1, &vao); GLERR
		if (vao == 0)
		{
			throw std::logic_error("VAO could not be created.");
		}
	GLuint vbo;
	glGenBuffers(1, &vbo);
	if (vbo == 0)
	{
		glDeleteVertexArrays(1, &vao);
		throw std::logic_error("VBO could not be created.");
	}
	GLuint ibo;
	glGenBuffers(1, &ibo);
	if (ibo == 0)
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		throw std::logic_error("IBO could not be created.");
	}
	auto& vertices = mesh.m_vertices.getData();
	glBindVertexArray(vao);	GLERR
	glBindBuffer(GL_ARRAY_BUFFER, vbo); GLERR
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); GLERR
	glBufferData(GL_ARRAY_BUFFER, sizeof(SCM::VertexData) * mesh.m_vertices.getData().size(), reinterpret_cast<const GLvoid*>(mesh.m_vertices.getData().data()), GL_STREAM_DRAW); GLERR
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(SCM::index) * mesh.m_indices.size(), reinterpret_cast<const GLvoid*>(mesh.m_indices.data()), GL_STATIC_DRAW); GLERR
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SCM::VertexData), reinterpret_cast<void*>(offsetof(SCM::VertexData, m_position))); GLERR
	glEnableVertexAttribArray(0); GLERR
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SCM::VertexData), reinterpret_cast<void*>(offsetof(SCM::VertexData, m_uv))); GLERR
	glEnableVertexAttribArray(1); GLERR
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(SCM::VertexData), reinterpret_cast<void*>(offsetof(SCM::VertexData, m_normal))); GLERR
	glEnableVertexAttribArray(2); GLERR
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SCM::VertexData), reinterpret_cast<void*>(offsetof(SCM::VertexData, m_tangent))); GLERR
	glEnableVertexAttribArray(3); GLERR
	glBindBuffer(GL_ARRAY_BUFFER, 0); GLERR
	glBindVertexArray(0); GLERR
	return std::make_shared<VAO>(vbo, ibo, vao, mesh.m_indices.size());
}

void GLUtils::updateVAO(std::shared_ptr<VAO>& vao, const SCM::MeshData & mesh)
{
	glBindBuffer(GL_ARRAY_BUFFER, vao->vbo);
	//Orphane the old buffer through creating a new one
	glBufferData(GL_ARRAY_BUFFER, sizeof(SCM::VertexData) * mesh.m_vertices.getData().size(), 0, GL_STREAM_DRAW);
	//Fill the new buffer with updated data
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SCM::VertexData) * mesh.m_vertices.getData().size(), reinterpret_cast<const void*>(mesh.m_vertices.getData().data()));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

std::shared_ptr<ShaderProgram> GLUtils::createShaderProgram(const std::string& vspath, const std::string& fspath)
{
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint program;
	std::string vertexCode;			
	std::string fragmentCode;		
std::ifstream vShaderFile;
std::ifstream fShaderFile;
vShaderFile.exceptions(std::ifstream::badbit);
fShaderFile.exceptions(std::ifstream::badbit);
try
{
	vShaderFile.open(vspath);
	if (!vShaderFile.is_open())
		throw std::invalid_argument("Vertex shader file not found.");
	fShaderFile.open(fspath);
	if (!fShaderFile.is_open())
		throw std::invalid_argument("Fragment shader file not found.");
	std::stringstream vShaderStream, fShaderStream;
	vShaderStream << vShaderFile.rdbuf();
	fShaderStream << fShaderFile.rdbuf();
	vShaderFile.close();
	fShaderFile.close();
	vertexCode = vShaderStream.str();
	fragmentCode = fShaderStream.str();
}
catch (const std::exception& ex)
{
	std::string errmsg;
	errmsg.append("Error: Shader files couldn't be read:\n");
	errmsg.append(ex.what());
	throw std::logic_error(errmsg.c_str());
}
const GLchar* vShaderCode = vertexCode.c_str();
const GLchar* fShaderCode = fragmentCode.c_str();
GLint success;
GLchar infoLog[512];
vertexShader = glCreateShader(GL_VERTEX_SHADER); GLERR
glShaderSource(vertexShader, 1, &vShaderCode, NULL); GLERR
glCompileShader(vertexShader); GLERR
glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); GLERR
if (!success)
{
	glGetShaderInfoLog(vertexShader, 512, NULL, infoLog); GLERR
		std::string errmsg;
	errmsg.append("Compiler error in vertex shader:\n");
	errmsg.append(infoLog);
	glDeleteShader(vertexShader); GLERR
		throw std::logic_error(errmsg.c_str());
}
fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); GLERR
glShaderSource(fragmentShader, 1, &fShaderCode, NULL); GLERR
glCompileShader(fragmentShader); GLERR
glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success); GLERR
if (!success)
{
	glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog); GLERR
		std::string errmsg;
	errmsg.append("Compiler error in fragment shader:\n");
	errmsg.append(infoLog);
	glDeleteShader(vertexShader); GLERR
		glDeleteShader(fragmentShader); GLERR
		throw std::logic_error(errmsg.c_str());
}
program = glCreateProgram(); GLERR
glAttachShader(program, vertexShader); GLERR
glAttachShader(program, fragmentShader); GLERR
glLinkProgram(program); GLERR
glGetProgramiv(program, GL_LINK_STATUS, &success); GLERR
if (!success)
{
	glGetProgramInfoLog(program, 512, NULL, infoLog); GLERR
		std::string errmsg;
	errmsg.append("Linker error in program:\n");
	errmsg.append(infoLog);
	glDetachShader(program, vertexShader); GLERR
		glDetachShader(program, fragmentShader); GLERR
		glDeleteShader(vertexShader); GLERR
		glDeleteShader(fragmentShader); GLERR
		throw std::logic_error(errmsg.c_str());
}
glDetachShader(program, vertexShader); GLERR
glDetachShader(program, fragmentShader); GLERR
glDeleteShader(vertexShader); GLERR
glDeleteShader(fragmentShader); GLERR
return std::make_shared<ShaderProgram>(program);
}

std::shared_ptr<ShaderProgram> GLUtils::createShaderProgram(const std::string & vspath, const std::string & fspath, const std::string & gspath)
{
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint geometryShader;
	GLuint program;
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::badbit);
	try
	{
		vShaderFile.open(vspath);
		if (!vShaderFile.is_open())
			throw std::invalid_argument("Vertex shader file not found.");
		fShaderFile.open(fspath);
		if (!fShaderFile.is_open())
			throw std::invalid_argument("Fragment shader file not found.");
		gShaderFile.open(gspath);
		if (!gShaderFile.is_open())
			throw std::invalid_argument("Geometry shader file not found.");
		std::stringstream vShaderStream, fShaderStream, gShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		gShaderStream << gShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		gShaderFile.close();
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		geometryCode = gShaderStream.str();
	}
	catch (const std::exception& ex)
	{
		std::string errmsg;
		errmsg.append("Error: Shader files couldn't be read:\n");
		errmsg.append(ex.what());
		throw std::logic_error(errmsg.c_str());
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();
	const GLchar* gShaderCode = geometryCode.c_str();
	GLint success;
	GLchar infoLog[512];
	vertexShader = glCreateShader(GL_VERTEX_SHADER); GLERR
	glShaderSource(vertexShader, 1, &vShaderCode, NULL); GLERR
	glCompileShader(vertexShader); GLERR
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); GLERR
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog); GLERR
		std::string errmsg;
		errmsg.append("Compiler error in vertex shader:\n");
		errmsg.append(infoLog);
		glDeleteShader(vertexShader); GLERR
		throw std::logic_error(errmsg.c_str());
	}
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); GLERR
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL); GLERR
	glCompileShader(fragmentShader); GLERR
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success); GLERR
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog); GLERR
		std::string errmsg;
		errmsg.append("Compiler error in fragment shader:\n");
		errmsg.append(infoLog);
		glDeleteShader(vertexShader); GLERR
		glDeleteShader(fragmentShader); GLERR
		throw std::logic_error(errmsg.c_str());
	}
	geometryShader = glCreateShader(GL_GEOMETRY_SHADER); GLERR
	glShaderSource(geometryShader, 1, &gShaderCode, NULL); GLERR
	glCompileShader(geometryShader); GLERR
	glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success); GLERR
	if (!success)
	{
		glGetShaderInfoLog(geometryShader, 512, NULL, infoLog); GLERR
			std::string errmsg;
		errmsg.append("Compiler error in geometry shader:\n");
		errmsg.append(infoLog);
		glDeleteShader(geometryShader); GLERR
		glDeleteShader(vertexShader); GLERR
		glDeleteShader(fragmentShader); GLERR
		throw std::logic_error(errmsg.c_str());
	}
	program = glCreateProgram(); GLERR
	glAttachShader(program, vertexShader); GLERR
	glAttachShader(program, fragmentShader); GLERR
	glAttachShader(program, geometryShader); GLERR
	glLinkProgram(program); GLERR
	glGetProgramiv(program, GL_LINK_STATUS, &success); GLERR
	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog); GLERR
			std::string errmsg;
		errmsg.append("Linker error in program:\n");
		errmsg.append(infoLog);
		glDetachShader(program, vertexShader); GLERR
		glDetachShader(program, fragmentShader); GLERR
		glDetachShader(program, geometryShader); GLERR
		glDeleteShader(vertexShader); GLERR
		glDeleteShader(fragmentShader); GLERR
		glDeleteShader(geometryShader); GLERR
		throw std::logic_error(errmsg.c_str());
	}
	glDetachShader(program, vertexShader); GLERR
	glDetachShader(program, fragmentShader); GLERR
	glDetachShader(program, geometryShader); GLERR
	glDeleteShader(vertexShader); GLERR
	glDeleteShader(fragmentShader); GLERR
	glDeleteShader(geometryShader); GLERR
	return std::make_shared<ShaderProgram>(program);
}

RenderTarget GLUtils::createRenderTargetRbuf(GLsizei width, GLsizei height, GLenum internalformat, GLenum attachment)
{
	auto p = createRenderBuffer(width, height, internalformat);
	return RenderTarget(p, attachment);
}

RenderTarget GLUtils::createRenderTargetTex(GLsizei width, GLsizei height, GLenum internalformat, GLenum attachment)
{
	auto p = createRenderTexture(width, height, internalformat);
	return RenderTarget(p, attachment);
}

std::shared_ptr<FrameBuffer> GLUtils::createFrameBuffer(std::vector<RenderTarget> colorTargets, RenderTarget depthTarget)
{
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	std::vector<GLenum> drawbuffers;
	for (size_t i = 0; i < colorTargets.size(); ++i)
	{
		if (colorTargets[i].isTex)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, colorTargets[i].attachment, GL_TEXTURE_2D, colorTargets[i].tex->tex, 0);
			drawbuffers.push_back(colorTargets[i].attachment);
		}
		else
		{
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, colorTargets[i].attachment, GL_RENDERBUFFER, colorTargets[i].rb->rbid);
			drawbuffers.push_back(colorTargets[i].attachment);
		}
	}
	if (depthTarget.attachment != GL_INVALID_ENUM)
	{
		if (depthTarget.isTex)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, depthTarget.attachment, GL_TEXTURE_2D, depthTarget.tex->tex, 0);
		}
		else
		{
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, depthTarget.attachment, GL_RENDERBUFFER, depthTarget.rb->rbid);
		}
	}

	GLenum fbstate;
	if (checkFBO(&fbstate))
	{
		glDrawBuffers(drawbuffers.size(), drawbuffers.data());
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return std::make_shared<FrameBuffer>(fbo, colorTargets, depthTarget, fbstate);
}

std::shared_ptr<FrameBuffer> GLUtils::createFrameBuffer(const FrameBufferDesc & fdesc)
{
	std::vector<RenderTarget> colorTargets;
	colorTargets.reserve(fdesc.colorTargets.size());
	RenderTarget depthTarget;
	for (size_t i = 0; i < fdesc.colorTargets.size(); i++)
	{
		if (fdesc.colorTargets[i].texture)
		{
			colorTargets.push_back(createRenderTargetTex(fdesc.colorTargets[i].width, fdesc.colorTargets[i].height, fdesc.colorTargets[i].internalformat, fdesc.colorTargets[i].attachment));
		}
		else
		{
			colorTargets.push_back(createRenderTargetRbuf(fdesc.colorTargets[i].width, fdesc.colorTargets[i].height, fdesc.colorTargets[i].internalformat, fdesc.colorTargets[i].attachment));
		}
	}
	if (fdesc.depthTarget.texture)
	{
		depthTarget = createRenderTargetTex(fdesc.depthTarget.width, fdesc.depthTarget.height, fdesc.depthTarget.internalformat, fdesc.depthTarget.attachment);
	}
	else
	{
		depthTarget = createRenderTargetRbuf(fdesc.depthTarget.width, fdesc.depthTarget.height, fdesc.depthTarget.internalformat, fdesc.depthTarget.attachment);
	}
	return createFrameBuffer(colorTargets, depthTarget);
}

std::shared_ptr<RenderBuffer> GLUtils::createRenderBuffer(GLsizei width, GLsizei height, GLenum internalformat)
{
	GLuint rid;
	glGenRenderbuffers(1, &rid);
	glBindRenderbuffer(GL_RENDERBUFFER, rid);
	glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
	return std::make_shared<RenderBuffer>(rid);
}

std::shared_ptr<Texture2D> GLUtils::createRenderTexture(GLsizei width, GLsizei height, GLenum internalformat)
{
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexStorage2D(GL_TEXTURE_2D, 1, internalformat, width, height);
	return std::make_shared<Texture2D>(id);
}

bool GLUtils::checkFBO(GLenum* result)
{
	auto res = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (res == GL_FRAMEBUFFER_COMPLETE)
	{
		*result = res;
		return true;
	}
	else
	{
		*result = res;
		return false;
	}
}

std::string GLUtils::getFrameBufferErrorMessage(GLenum state)
{
	return std::string();
}

VAO::VAO(GLuint vbo, GLuint ibo, GLuint vao, GLsizei indexct) :
	vbo(vbo),
	ibo(ibo),
	vao(vao),
	indexCount(indexct)
{
}

VAO::~VAO()
{
	glDeleteBuffers(1, &ibo);	GLERR
	glDeleteBuffers(1, &vbo);	GLERR
	glDeleteVertexArrays(1, &vao);	GLERR
}

void VAO::bind()
{
	if (vao != 0)
		glBindVertexArray(vao);	GLERR
}

void VAO::unbind()
{
	glBindVertexArray(0); GLERR
}

Texture2D::Texture2D(GLuint texture) :
	tex(texture),
	tu(0)
{
}

Texture2D::~Texture2D()
{
	unbind();
	glDeleteTextures(1, &tex); GLERR
}

void Texture2D::bind(GLuint textureUnit)
{
	if (tex != 0)
	{
		glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(textureUnit)); GLERR
		tu = textureUnit;
		glBindTexture(GL_TEXTURE_2D, tex); GLERR
	}
}

void Texture2D::unbind()
{
	glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(tu)); GLERR
	glBindTexture(GL_TEXTURE_2D, 0); GLERR
}

ShaderProgram::ShaderProgram(GLuint program) :
	prog(program)
{
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(prog); GLERR
}

void ShaderProgram::use()
{
	GLint current;
	glGetIntegerv(GL_CURRENT_PROGRAM, &current); GLERR
	if (current != prog && prog != 0)
		glUseProgram(prog); GLERR
}

TextureCube::TextureCube(GLuint texture) :
	tex(texture),
	tu(0)
{}

TextureCube::~TextureCube()
{
	unbind();
	glDeleteTextures(1, &tex); GLERR
}

void TextureCube::bind(GLuint textureUnit)
{
	if (tex != 0)
	{
		glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(textureUnit)); GLERR
		tu = textureUnit;
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex); GLERR
	}
}

void TextureCube::unbind()
{
	glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(tu)); GLERR
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0); GLERR
}

void FrameBuffer::bind(GLenum target)
{
	glBindFramebuffer(target, this->fbo);
}

void FrameBuffer::unbind(GLenum target)
{
	glBindFramebuffer(target, 0);
}
