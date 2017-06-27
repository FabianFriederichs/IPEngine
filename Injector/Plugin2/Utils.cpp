#include "Utils.h"

//std::shared_ptr<Texture2D> GLUtils::loadGLTexture(const std::string& path)
//{
//	GLsizei width;
//	GLsizei height;
//	GLsizei channels;
//	unsigned char* image = SOIL_load_image(path.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);
//	GLuint texid = 0;
//	if (image == nullptr)
//	{
//		throw std::logic_error("Texture file coudn't be read.");
//	}
//	else
//	{
//		glGenTextures(1, &texid); GLERR
//		if (texid == 0)
//		{
//			throw std::logic_error("OpenGL texture object creation failed.");
//		}
//		glBindTexture(GL_TEXTURE_2D, texid); GLERR
//		glTexImage2D(
//			GL_TEXTURE_2D,		
//			0,					
//			GL_RGBA8,			
//			width,				
//			height,				
//			0,					
//			GL_RGBA,			
//			GL_UNSIGNED_BYTE,	
//			image				
//		);
//		if (checkglerror())
//		{
//			glDeleteTextures(1, &texid);
//			SOIL_free_image_data(image);
//			throw std::logic_error("Error. Could not buffer texture data.");
//		}
//		glGenerateMipmap(GL_TEXTURE_2D); GLERR
//		glBindTexture(GL_TEXTURE_2D, 0); GLERR
//		SOIL_free_image_data(image);
//	}
//	return std::make_shared<Texture2D>(texid);
//}

std::shared_ptr<VAO> GLUtils::createVAO(const OBJMesh& mesh)
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
	glBindVertexArray(vao);	GLERR
		glBindBuffer(GL_ARRAY_BUFFER, vbo); GLERR
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); GLERR
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh.vertices.size(), reinterpret_cast<const GLvoid*>(mesh.vertices.data()), GL_STATIC_DRAW); GLERR
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Index) * mesh.indices.size(), reinterpret_cast<const GLvoid*>(mesh.indices.data()), GL_STATIC_DRAW); GLERR
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position))); GLERR
		glEnableVertexAttribArray(0); GLERR											
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv))); GLERR
		glEnableVertexAttribArray(1); GLERR
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal))); GLERR
		glEnableVertexAttribArray(2); GLERR
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tangent))); GLERR
		glEnableVertexAttribArray(3); GLERR
		glBindBuffer(GL_ARRAY_BUFFER, 0); GLERR
		glBindVertexArray(0); GLERR
		return std::make_shared<VAO>(vbo, ibo, vao, mesh.indices.size());
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
