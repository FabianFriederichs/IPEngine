#include "Utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
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
	return RenderTarget(p, attachment, width, height);
}

RenderTarget GLUtils::createRenderTargetTex(GLsizei width, GLsizei height, GLenum internalformat, GLenum attachment, int miplevels)
{
	auto p = createRenderTexture(width, height, internalformat, miplevels);
	return RenderTarget(p, attachment, width, height, miplevels);
}

RenderTarget GLUtils::createRenderTargetCube(GLsizei width, GLsizei height, GLenum internalformat, GLenum attachment, int miplevels)
{
	auto p = createRenderTextureCube(width, height, internalformat, miplevels);
	return RenderTarget(p, attachment, width, height, miplevels);
}

std::shared_ptr<FrameBuffer> GLUtils::createFrameBuffer(const RenderTargetSet& rtset)
{
	GLuint fbo;
	glGenFramebuffers(1, &fbo);	
	auto f = std::make_shared<FrameBuffer>(fbo);
	f->bind(GL_FRAMEBUFFER);
	f->attachRenderTargetSet(rtset);
	f->unbind(GL_FRAMEBUFFER);
	return f;
}

std::shared_ptr<FrameBuffer> GLUtils::createFrameBuffer(const FrameBufferDesc & fdesc)
{	
	return createFrameBuffer(createRenderTargetSet(fdesc));
}

std::shared_ptr<FrameBuffer> GLUtils::createFrameBuffer()
{
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	return std::make_shared<FrameBuffer>(fbo);
}

RenderTargetSet GLUtils::createRenderTargetSet(const FrameBufferDesc & fdesc)
{
	RenderTargetSet rtset;
	rtset.colorTargets.reserve(fdesc.colorTargets.size());
	for (size_t i = 0; i < fdesc.colorTargets.size(); i++)
	{
		if (fdesc.colorTargets[i].type == RenderTargetType::Texture2D)
		{
			rtset.colorTargets.push_back(createRenderTargetTex(fdesc.colorTargets[i].width, fdesc.colorTargets[i].height, fdesc.colorTargets[i].internalformat, fdesc.colorTargets[i].attachment, fdesc.colorTargets[i].miplevels));
		}
		else if (fdesc.colorTargets[i].type == RenderTargetType::RenderBuffer)
		{
			rtset.colorTargets.push_back(createRenderTargetRbuf(fdesc.colorTargets[i].width, fdesc.colorTargets[i].height, fdesc.colorTargets[i].internalformat, fdesc.colorTargets[i].attachment));
		}
		else if (fdesc.colorTargets[i].type == RenderTargetType::TextureCube)
		{
			rtset.colorTargets.push_back(createRenderTargetCube(fdesc.colorTargets[i].width, fdesc.colorTargets[i].height, fdesc.colorTargets[i].internalformat, fdesc.colorTargets[i].attachment, fdesc.colorTargets[i].miplevels));
		}
		else if (fdesc.colorTargets[i].type == RenderTargetType::Empty)
		{
			rtset.colorTargets.push_back(RenderTarget());
		}
		else
		{
			rtset.colorTargets.push_back(RenderTarget());
		}
	}
	if (fdesc.depthTarget.type == RenderTargetType::Texture2D)
	{
		rtset.depthTarget = createRenderTargetTex(fdesc.depthTarget.width, fdesc.depthTarget.height, fdesc.depthTarget.internalformat, fdesc.depthTarget.attachment, fdesc.depthTarget.miplevels);
	}
	else if (fdesc.depthTarget.type == RenderTargetType::RenderBuffer)
	{
		rtset.depthTarget = createRenderTargetRbuf(fdesc.depthTarget.width, fdesc.depthTarget.height, fdesc.depthTarget.internalformat, fdesc.depthTarget.attachment);
	}
	else if (fdesc.depthTarget.type == RenderTargetType::TextureCube)
	{
		rtset.depthTarget = createRenderTargetCube(fdesc.depthTarget.width, fdesc.depthTarget.height, fdesc.depthTarget.internalformat, fdesc.depthTarget.attachment, fdesc.depthTarget.miplevels);
	}
	else if (fdesc.depthTarget.type == RenderTargetType::Empty)
	{
		rtset.depthTarget = RenderTarget();
	}
	else
	{
		rtset.depthTarget = RenderTarget();
	}
	return rtset;
}

std::shared_ptr<RenderBuffer> GLUtils::createRenderBuffer(GLsizei width, GLsizei height, GLenum internalformat)
{
	GLuint rid;
	glGenRenderbuffers(1, &rid);
	glBindRenderbuffer(GL_RENDERBUFFER, rid);
	glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
	return std::make_shared<RenderBuffer>(rid);
}

std::shared_ptr<Texture2D> GLUtils::createRenderTexture(GLsizei width, GLsizei height, GLenum internalformat, int miplevels)
{
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexStorage2D(GL_TEXTURE_2D, miplevels, internalformat, width, height);
	return std::make_shared<Texture2D>(id);
}

std::shared_ptr<TextureCube> GLUtils::createRenderTextureCube(GLsizei width, GLsizei height, GLenum internalformat, int miplevels)
{
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	glTexStorage2D(GL_TEXTURE_CUBE_MAP, miplevels, internalformat, width, height);
	return std::make_shared<TextureCube>(id);
}

bool GLUtils::checkFBO(GLenum* result)
{
	auto res = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (res == GL_FRAMEBUFFER_COMPLETE)
	{
		if(result)
			*result = res;
		return true;
	}
	else
	{
		if(result)
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

void Texture2D::setTexParams(GLint minf, GLint magf, GLint wraps, GLint wrapt, float maxAniso)
{
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minf);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magf);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wraps);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapt);
	float impmax;
	if (maxAniso > 0)
	{
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &impmax);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso >= impmax ? impmax : maxAniso);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::setBorderColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{	
	GLfloat fv[]{ r, g, b, a };
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, fv);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::genMipMaps()
{
	glBindTexture(GL_TEXTURE_2D, tex);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
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
	currentTu = 0;
	GLint current;
	glGetIntegerv(GL_CURRENT_PROGRAM, &current); GLERR
	if (current != prog && prog != 0)
		glUseProgram(prog); GLERR
}

TextureCube::TextureCube(GLuint texture) :
	tex(texture),
	tu(0)
{

}

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

void TextureCube::setTexParams(GLint minf, GLint magf, GLint wraps, GLint wrapt, GLint wrapr, float maxAniso)
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minf);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magf);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wraps);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapt);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapr);
	float impmax;
	if (maxAniso > 0)
	{
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &impmax);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso >= impmax ? impmax : maxAniso);
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void FrameBuffer::bind(GLenum target)
{
	glBindFramebuffer(target, this->fbo);
}

void FrameBuffer::unbind(GLenum target)
{
	glBindFramebuffer(target, 0);
}

bool FrameBuffer::selectColorTargetMipmapLevel(size_t colorTargetIndex, GLint level)
{
	//only allow selection if this framebuffer is currently bound
	GLint currentFB;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFB);
	if (currentFB != fbo)
		return false;

	if (level >= rtset.colorTargets[colorTargetIndex].miplevels)
		return false;
	
	if (rtset.colorTargets[colorTargetIndex].type == RenderTargetType::Texture2D)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, rtset.colorTargets[colorTargetIndex].attachment, GL_TEXTURE_2D, rtset.colorTargets[colorTargetIndex].tex->tex, level);
	}
	else if (rtset.colorTargets[colorTargetIndex].type == RenderTargetType::RenderBuffer)
	{
		return false;
	}
	else if (rtset.colorTargets[colorTargetIndex].type == RenderTargetType::TextureCube)
	{
		glFramebufferTexture(GL_FRAMEBUFFER, rtset.colorTargets[colorTargetIndex].attachment, rtset.colorTargets[colorTargetIndex].ctex->tex, level);
	}
	else
	{
		return false;
	}	

	GLUtils::checkFBO(&state);
	if (state == GL_FRAMEBUFFER_COMPLETE)	
		return true;	
	return false;
}

bool FrameBuffer::selectDepthTargetMipmapLevel(GLint level)
{
	//only allow selection if this framebuffer is currently bound
	GLint currentFB;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFB);
	if (currentFB != fbo)
		return false;

	if (level >= rtset.depthTarget.miplevels)
		return false;

	if (rtset.depthTarget.type == RenderTargetType::Texture2D)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, rtset.depthTarget.attachment, GL_TEXTURE_2D, rtset.depthTarget.tex->tex, level);
	}
	else if (rtset.depthTarget.type == RenderTargetType::RenderBuffer)
	{
		return false;
	}
	else if (rtset.depthTarget.type == RenderTargetType::TextureCube)
	{
		glFramebufferTexture(GL_FRAMEBUFFER, rtset.depthTarget.attachment, rtset.depthTarget.ctex->tex, level);
	}
	else
	{
		return false;
	}

	GLUtils::checkFBO(&state);
	if (state == GL_FRAMEBUFFER_COMPLETE)
		return true;
	return false;
}

bool FrameBuffer::attachRenderTargetSet(const RenderTargetSet & rtset)
{
	GLint currentFB;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFB);
	if (currentFB != fbo)
		return false;
	//bind(GL_FRAMEBUFFER);
	std::vector<GLenum> drawbuffers;
	for (size_t i = 0; i < rtset.colorTargets.size(); ++i)
	{
		if(!rtset.colorTargets[i].active)
			continue;
		if (rtset.colorTargets[i].type == RenderTargetType::Texture2D)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, rtset.colorTargets[i].attachment, GL_TEXTURE_2D, rtset.colorTargets[i].tex->tex, 0);
			drawbuffers.push_back(rtset.colorTargets[i].attachment);
		}
		else if (rtset.colorTargets[i].type == RenderTargetType::RenderBuffer)
		{
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, rtset.colorTargets[i].attachment, GL_RENDERBUFFER, rtset.colorTargets[i].rb->rbid);
			drawbuffers.push_back(rtset.colorTargets[i].attachment);
		}
		else if (rtset.colorTargets[i].type == RenderTargetType::TextureCube)
		{
			glFramebufferTexture(GL_FRAMEBUFFER, rtset.colorTargets[i].attachment, rtset.colorTargets[i].ctex->tex, 0);
			drawbuffers.push_back(rtset.colorTargets[i].attachment);
		}
		else if (rtset.colorTargets[i].type == RenderTargetType::Empty)
		{
			glDeleteFramebuffers(1, &fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			throw std::logic_error("Error creating framebuffer: empty color targets are not allowed.");
		}
		else
		{
			glDeleteFramebuffers(1, &fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			throw std::logic_error("Error creating framebuffer: render target type is not supported.");
		}
	}
	if (rtset.depthTarget.type != RenderTargetType::Empty)
	{
		if (rtset.depthTarget.type == RenderTargetType::Texture2D)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, rtset.depthTarget.attachment, GL_TEXTURE_2D, rtset.depthTarget.tex->tex, 0);
		}
		else if (rtset.depthTarget.type == RenderTargetType::RenderBuffer)
		{
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, rtset.depthTarget.attachment, GL_RENDERBUFFER, rtset.depthTarget.rb->rbid);
		}
		else if (rtset.depthTarget.type == RenderTargetType::TextureCube)
		{
			glFramebufferTexture(GL_FRAMEBUFFER, rtset.depthTarget.attachment, rtset.depthTarget.ctex->tex, 0);
		}
		else
		{
			glDeleteFramebuffers(1, &fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			throw std::logic_error("Error creating framebuffer: render target type is not supported.");
		}
	}

	if (GLUtils::checkFBO(&state))
	{
		this->rtset = rtset;
		glDrawBuffers(drawbuffers.size(), drawbuffers.data());
		return true;
	}
	//unbind(GL_FRAMEBUFFER);
	
	return false;
}
