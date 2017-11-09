#ifndef _UTILS_H_
#define _UTILS_H_

inline size_t index2d(size_t x, size_t y, size_t width)
{
	return y * width + x;
}

#include "libheaders.h"
#include <string>
#include <vector>
#include "ISimpleContentModule_API.h"
#include <memory>
#include "glerror.h"

class VAO;
class Texture2D;
class ShaderProgram;

class GLUtils
{
public:
	GLUtils() = delete;					
	GLUtils(const GLUtils&) = delete;	
	GLUtils(GLUtils&&) = delete;

	//static std::shared_ptr<Texture2D> loadGLTexture(const std::string& path);
	static std::shared_ptr<VAO> createVAO(const SCM::MeshData& mesh);
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

	template<class T>
	inline void setUniform(const std::string name, T stuff);
	template<class T>
	inline void setUniform(const std::string name, T stuff, const GLboolean transpose);

private:
	GLint getUniformLocation(std::string name)
	{
		return glGetUniformLocation(this->prog, name.c_str()); GLERR
	}
};



#endif