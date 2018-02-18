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
#include "Primitives.h"

class VAO;
class Texture2D;
class TextureCube;
class ShaderProgram;
class RenderTarget;
class RenderBuffer;
class FrameBuffer;
struct FrameBufferDesc;
struct RenderTargetDesc;

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
	static std::shared_ptr<ShaderProgram> createShaderProgram(const std::string& vspath, const std::string& fspath, const std::string& gspath);

	//add support for cube maps and ms when needed
	static RenderTarget createRenderTargetRbuf(GLsizei width, GLsizei height, GLenum internalformat, GLenum attachment);
	static RenderTarget createRenderTargetTex(GLsizei width, GLsizei height, GLenum internalformat, GLenum attachment);
	static RenderTarget createRenderTargetCube(GLsizei width, GLsizei height, GLenum internalformat, GLenum attachment);
	static std::shared_ptr<FrameBuffer> createFrameBuffer(std::vector<RenderTarget> colorTargets, RenderTarget depthTarget);
	static std::shared_ptr<FrameBuffer> createFrameBuffer(const FrameBufferDesc& fdesc);

private:
	static std::shared_ptr<RenderBuffer> createRenderBuffer(GLsizei width, GLsizei height, GLenum internalformat);
	static std::shared_ptr<Texture2D> createRenderTexture(GLsizei width, GLsizei height, GLenum internalformat);
	static std::shared_ptr<TextureCube> createRenderTextureCube(GLsizei width, GLsizei height, GLenum internalformat);
	static bool checkFBO(GLenum* result);
	static std::string getFrameBufferErrorMessage(GLenum state);
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
	void setTexParams(GLint minf = GL_LINEAR, GLint magf = GL_LINEAR, GLint wraps = GL_REPEAT, GLint wrapt = GL_REPEAT, float maxAniso = 0);
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
	void setTexParams(GLint minf = GL_LINEAR, GLint magf = GL_LINEAR, GLint wraps = GL_CLAMP_TO_EDGE, GLint wrapt = GL_CLAMP_TO_EDGE, GLint wrapr = GL_CLAMP_TO_EDGE, float maxAniso = 0);
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

class RenderBuffer
{
public:
	RenderBuffer() :
		rbid(0)
	{}
	RenderBuffer(GLuint id) :
		rbid(id)
	{}
	GLuint rbid;

	~RenderBuffer()
	{
		if (rbid)
			glDeleteRenderbuffers(1, &rbid);
	}
};

enum class RenderTargetType
{
	//TODO: add support for commented target types!
	Empty,
	RenderBuffer,
	//RenderBufferMS,
	Texture2D,
	//Texture2DMip,
	//Texture2DMS,
	TextureCube
	//TextureCubeMip
};

class RenderTarget
{
public:
	RenderTarget() :
		attachment(GL_INVALID_ENUM),
		rb(),
		type(RenderTargetType::Empty)
	{}
	~RenderTarget()
	{}
	RenderTarget(std::shared_ptr<Texture2D> _tex, GLenum _attachment) :
		tex(_tex),
		attachment(_attachment),
		type(RenderTargetType::Texture2D)
	{}
	RenderTarget(std::shared_ptr<RenderBuffer> _rb, GLenum _attachment) :
		rb(_rb),
		attachment(_attachment),
		type(RenderTargetType::RenderBuffer)
	{}
	RenderTarget(std::shared_ptr<TextureCube> _ctex, GLenum _attachment) :
		ctex(_ctex),
		attachment(_attachment),
		type(RenderTargetType::TextureCube)
	{}
	RenderTarget(const RenderTarget& other) 
	{
		if (other.type == RenderTargetType::Empty)
		{
			type = RenderTargetType::Empty;
			attachment = GL_INVALID_ENUM;
		}
		if (other.type == RenderTargetType::Texture2D)
		{
			attachment = other.attachment;
			tex = other.tex;
			type = RenderTargetType::Texture2D;
		}
		else if (other.type == RenderTargetType::RenderBuffer)
		{
			attachment = other.attachment;
			rb = other.rb;
			type = RenderTargetType::RenderBuffer;
		}
		else if (other.type == RenderTargetType::TextureCube)
		{
			attachment = other.attachment;
			ctex = other.ctex;
			type = RenderTargetType::TextureCube;
		}
		else //cube map follows
		{
			
		}
	}
	RenderTarget& operator=(const RenderTarget& other)
	{
		if (this == &other)
			return *this;
		if (other.type == RenderTargetType::Empty)
		{
			type = RenderTargetType::Empty;
			attachment = GL_INVALID_ENUM;
		}
		if (other.type == RenderTargetType::Texture2D)
		{
			attachment = other.attachment;
			tex = other.tex;
			type = RenderTargetType::Texture2D;
		}
		else if (other.type == RenderTargetType::RenderBuffer)
		{
			attachment = other.attachment;
			rb = other.rb;
			type = RenderTargetType::RenderBuffer;
		}
		else if (other.type == RenderTargetType::TextureCube)
		{
			attachment = other.attachment;
			ctex = other.ctex;
			type = RenderTargetType::TextureCube;
		}
		else //cube map follows
		{

		}
		return *this;
	}
	RenderTargetType type;
	std::shared_ptr<Texture2D> tex;
	std::shared_ptr<RenderBuffer> rb;
	std::shared_ptr<TextureCube> ctex;
	GLenum attachment;
};

class FrameBuffer
{
public:
	FrameBuffer() : 
		fbo(0),
		depthTarget(),
		colorTargets()
	{}
	FrameBuffer(GLuint _fbo, const std::vector<RenderTarget>& _ct, const RenderTarget& _dt, GLenum _state) :
		state(_state),
		depthTarget(),
		colorTargets()
	{
		if (_state == GL_FRAMEBUFFER_COMPLETE)
		{
			fbo = _fbo;
			depthTarget = _dt;
			colorTargets = _ct;
		}
	}
	~FrameBuffer()
	{
		if (fbo)
			glDeleteFramebuffers(1, &fbo);
	}
	void bind(GLenum target);
	void unbind(GLenum target);
	GLuint fbo;
	std::vector<RenderTarget> colorTargets;
	RenderTarget depthTarget;
	GLenum state;
	bool isComplete()
	{
		return state == GL_FRAMEBUFFER_COMPLETE;
	}
};

struct RenderTargetDesc
{
	RenderTargetDesc() :
		width(0),
		height(0),
		internalformat(GL_INVALID_ENUM),
		attachment(GL_INVALID_ENUM),
		type(RenderTargetType::Empty),
		samples(0)
	{}

	RenderTargetDesc(GLsizei _width,
					 GLsizei _height,
					 GLenum _internalformat,
					 GLenum _attachment,
					 RenderTargetType _type,
					 int samples = 0) :
		width(_width),
		height(_height),
		internalformat(_internalformat),
		attachment(_attachment),
		type(_type),
		samples(samples)
	{}

	~RenderTargetDesc()
	{}

	GLsizei width;
	GLsizei height;
	GLenum internalformat;
	GLenum attachment;
	RenderTargetType type;
	int samples;
};

struct FrameBufferDesc
{
	std::vector<RenderTargetDesc> colorTargets;
	RenderTargetDesc depthTarget;
};
#endif