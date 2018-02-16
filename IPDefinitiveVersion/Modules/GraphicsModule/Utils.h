#ifndef _UTILS_H_
#define _UTILS_H_

inline size_t index2d(size_t x, size_t y, size_t width)
{
	return y * width + x;
}

#include "libheaders.h"
#include <string>
#include <vector>
#include <ISimpleContentModule_API.h>
#include <memory>
#include "glerror.h"

class VAO;
class Texture2D;
class TextureCube;
class ShaderProgram;

class GLUtils
{
public:
	GLUtils() = delete;					
	GLUtils(const GLUtils&) = delete;	
	GLUtils(GLUtils&&) = delete;

	static std::shared_ptr<Texture2D> loadGLTexture(const std::string& path ,bool genMipMaps);
	static std::shared_ptr<TextureCube> loadGLCubeTexture(const std::string& path_px,
														  const std::string& path_nx,
														  const std::string& path_py,
														  const std::string& path_ny,
														  const std::string& path_pz,
														  const std::string& path_nz,
														  bool genMipMaps);
	static std::shared_ptr<Texture2D> loadGLTextureHDR(const std::string& path, bool genMipMaps, bool halfprecision = true);
	static std::shared_ptr<TextureCube> loadGLCubeTextureHDR(const std::string& path_px,
															 const std::string& path_nx,
															 const std::string& path_py,
															 const std::string& path_ny,
															 const std::string& path_pz,
															 const std::string& path_nz,
															 bool genMipMaps,
															 bool halfprecision = true);
	
	static std::shared_ptr<VAO> createVAO(const SCM::MeshData& mesh);
	static std::shared_ptr<VAO> createDynamicVAO(const SCM::MeshData& mesh);
	static void updateVAO(std::shared_ptr<VAO>& vao, const SCM::MeshData& mesh);
	static std::shared_ptr<ShaderProgram> createShaderProgram(const std::string& vspath, const std::string& fspath);
};

class VAO
{
public:
	VAO(GLuint vbo, GLuint ibo, GLuint vao, GLsizei indexct);
	VAO() = default;
	~VAO();
	void bind();
	void unbind();

	GLuint vbo;
	GLuint ibo;
	GLuint vao;
	GLsizei indexCount;
};

class Texture2D
{
public:
	Texture2D(GLuint texture);
	~Texture2D();
	void bind(GLuint textureUnit = 0);
	void unbind();
	GLuint tex;
	GLuint tu;
	bool ishdr;
};

class TextureCube
{
public:
	TextureCube(GLuint texture);
	~TextureCube();
	void bind(GLuint textureUnit = 0);
	void unbind();
	GLuint tex;
	GLuint tu;
	bool ishdr;
};

class ShaderProgram
{
public:
	ShaderProgram(GLuint program);
	~ShaderProgram();
	void use();
	GLuint prog;

	bool isActive()
	{
		GLint progName = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &progName); GLERR
			if (progName != this->prog)
				return false;
		return true;
	}

	void setUniform(const std::string& name, GLfloat value);
	void setUniform(const std::string& name, const glm::vec2& value);
	void setUniform(const std::string& name, const glm::vec3& value);
	void setUniform(const std::string& name, const glm::vec4& value);

	void setUniform(const std::string& name, GLint value);
	void setUniform(const std::string& name, const glm::ivec2& value);
	void setUniform(const std::string& name, const glm::ivec3& value);
	void setUniform(const std::string& name, const glm::ivec4& value);

	void setUniform(const std::string& name, GLuint value);
	void setUniform(const std::string& name, const glm::uvec2& value);
	void setUniform(const std::string& name, const glm::uvec3& value);
	void setUniform(const std::string& name, const glm::uvec4& value);

	void setUniform(const std::string& name, const glm::mat2& value, bool transpose);
	void setUniform(const std::string& name, const glm::mat3& value, bool transpose);
	void setUniform(const std::string& name, const glm::mat4& value, bool transpose);

private:
	GLint getUniformLocation(const std::string& name)
	{
		return glGetUniformLocation(this->prog, name.c_str()); GLERR
	}
};

inline void ShaderProgram::setUniform(const std::string& name, GLfloat value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return;
	if (!isActive())
		glUseProgram(this->prog); GLERR
	glUniform1f(loc, value); GLERR
}

inline void ShaderProgram::setUniform(const std::string& name, const glm::vec2& value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return;
	if (!isActive())	
		glUseProgram(this->prog); GLERR	
	glUniform2fv(loc, 1, glm::value_ptr(value)); GLERR
}

inline void ShaderProgram::setUniform(const std::string& name, const glm::vec3& value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return;
	if (!isActive())
		glUseProgram(this->prog); GLERR
	glUniform3fv(loc, 1, glm::value_ptr(value)); GLERR
}

inline void ShaderProgram::setUniform(const std::string& name, const glm::vec4& value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return;
	if (!isActive())
		glUseProgram(this->prog); GLERR
	glUniform4fv(loc, 1, glm::value_ptr(value)); GLERR
}

inline void ShaderProgram::setUniform(const std::string& name, GLint value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return;
	if (!isActive())
		glUseProgram(this->prog); GLERR
	glUniform1i(loc, value); GLERR
}

inline void ShaderProgram::setUniform(const std::string& name, const glm::ivec2& value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return;
	if (!isActive())
		glUseProgram(this->prog); GLERR
	glUniform2iv(loc, 1, glm::value_ptr(value)); GLERR
}

inline void ShaderProgram::setUniform(const std::string& name, const glm::ivec3& value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return;
	if (!isActive())
		glUseProgram(this->prog); GLERR
	glUniform3iv(loc, 1, glm::value_ptr(value)); GLERR
}

inline void ShaderProgram::setUniform(const std::string& name, const glm::ivec4& value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return;
	if (!isActive())
		glUseProgram(this->prog); GLERR
	glUniform4iv(loc, 1, glm::value_ptr(value)); GLERR
}

inline void ShaderProgram::setUniform(const std::string& name, GLuint value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return;
	if (!isActive())
		glUseProgram(this->prog); GLERR
	glUniform1ui(loc, value); GLERR
}

inline void ShaderProgram::setUniform(const std::string& name, const glm::uvec2& value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return;
	if (!isActive())
		glUseProgram(this->prog); GLERR
	glUniform2uiv(loc, 1, glm::value_ptr(value)); GLERR
}

inline void ShaderProgram::setUniform(const std::string& name, const glm::uvec3& value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return;
	if (!isActive())
		glUseProgram(this->prog); GLERR
	glUniform3uiv(loc, 1, glm::value_ptr(value)); GLERR
}

inline void ShaderProgram::setUniform(const std::string& name, const glm::uvec4& value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return;
	if (!isActive())
		glUseProgram(this->prog); GLERR
	glUniform4uiv(loc, 1, glm::value_ptr(value)); GLERR
}

inline void ShaderProgram::setUniform(const std::string& name, const glm::mat2& value, bool transpose)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return;
	if (!isActive())
		glUseProgram(this->prog); GLERR
	glUniformMatrix2fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)); GLERR
}

inline void ShaderProgram::setUniform(const std::string& name, const glm::mat3& value, bool transpose)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return;
	if (!isActive())
		glUseProgram(this->prog); GLERR
	glUniformMatrix3fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)); GLERR
}

inline void ShaderProgram::setUniform(const std::string& name, const glm::mat4& value, bool transpose)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return;
	if (!isActive())
		glUseProgram(this->prog); GLERR
	glUniformMatrix4fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)); GLERR
}

#endif