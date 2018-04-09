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
struct RenderTargetSet;

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

	static RenderTarget createRenderTargetRbuf(GLsizei width, GLsizei height, GLenum internalformat, GLenum attachment);
	static RenderTarget createRenderTargetTex(GLsizei width, GLsizei height, GLenum internalformat, GLenum attachment, int miplevels = 1);
	static RenderTarget createRenderTargetCube(GLsizei width, GLsizei height, GLenum internalformat, GLenum attachment, int miplevels = 1);
	static std::shared_ptr<FrameBuffer> createFrameBuffer(const RenderTargetSet& rtset);
	static std::shared_ptr<FrameBuffer> createFrameBuffer(const FrameBufferDesc& fdesc);
	static std::shared_ptr<FrameBuffer> createFrameBuffer();
	static RenderTargetSet createRenderTargetSet(const FrameBufferDesc& fdesc);
	static bool checkFBO(GLenum* result);

private:
	static std::shared_ptr<RenderBuffer> createRenderBuffer(GLsizei width, GLsizei height, GLenum internalformat);
	static std::shared_ptr<Texture2D> createRenderTexture(GLsizei width, GLsizei height, GLenum internalformat, int miplevels);
	static std::shared_ptr<TextureCube> createRenderTextureCube(GLsizei width, GLsizei height, GLenum internalformat, int miplevels);	
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
	GLint currentTu;

	bool isActive()
	{
		GLint progName = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &progName); GLERR
		if (progName != this->prog)
			return false;
		return true;
	}

	bool bindTex(const char* name, Texture2D* tex);
	bool bindTex(const char* name, TextureCube* tex);
	bool occupyTex(const char* name);
	void resetTU(int newCurrentTU = 0);
	int getCurrentTU();

	bool setUniform(const char* name, GLfloat value);
	bool setUniform(const char* name, const glm::vec2& value);
	bool setUniform(const char* name, const glm::vec3& value);
	bool setUniform(const char* name, const glm::vec4& value);
			 
	bool setUniform(const char* name, GLint value);
	bool setUniform(const char* name, const glm::ivec2& value);
	bool setUniform(const char* name, const glm::ivec3& value);
	bool setUniform(const char* name, const glm::ivec4& value);
			 
	bool setUniform(const char* name, GLuint value);
	bool setUniform(const char* name, const glm::uvec2& value);
	bool setUniform(const char* name, const glm::uvec3& value);
	bool setUniform(const char* name, const glm::uvec4& value);
			
	bool setUniform(const char* name, const glm::mat2& value, bool transpose);
	bool setUniform(const char* name, const glm::mat3& value, bool transpose);
	bool setUniform(const char* name, const glm::mat4& value, bool transpose);

private:
	GLint getUniformLocation(const char* name)
	{
		return glGetUniformLocation(this->prog, name); GLERR
	}
};

inline bool ShaderProgram::bindTex(const char * name, Texture2D * tex)
{
	if (isActive() && getUniformLocation(name) != -1)
	{
		if(tex)
			tex->bind(currentTu);
		setUniform(name, currentTu);
		++currentTu;
		return true;		
	}
	return false;
}

inline bool ShaderProgram::occupyTex(const char * name)
{
	if (isActive() && getUniformLocation(name) != -1)
	{
		setUniform(name, currentTu);
		++currentTu;
		return true;
	}
	return false;
}

inline bool ShaderProgram::bindTex(const char * name, TextureCube * tex)
{
	if (isActive() && getUniformLocation(name) != -1)
	{
		if(tex)
			tex->bind(currentTu);
		setUniform(name, currentTu);
		++currentTu;
		return true;
	}
	return false;
}

inline void ShaderProgram::resetTU(int newCurrentTU)
{
	currentTu = newCurrentTU;
}

inline int ShaderProgram::getCurrentTU()
{
	return currentTu;
}



inline bool ShaderProgram::setUniform(const char* name, GLfloat value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return false;
	if (!isActive())
		return false;
	glUniform1f(loc, value); GLERR
	return true;
}

inline bool ShaderProgram::setUniform(const char* name, const glm::vec2& value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return false;
	if (!isActive())	
		return false;	
	glUniform2fv(loc, 1, glm::value_ptr(value)); GLERR
	return true;
}

inline bool ShaderProgram::setUniform(const char* name, const glm::vec3& value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return false;
	if (!isActive())
		return false;
	glUniform3fv(loc, 1, glm::value_ptr(value)); GLERR
	return true;
}

inline bool ShaderProgram::setUniform(const char* name, const glm::vec4& value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return false;
	if (!isActive())
		return false;
	glUniform4fv(loc, 1, glm::value_ptr(value)); GLERR
	return true;
}

inline bool ShaderProgram::setUniform(const char* name, GLint value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return false;
	if (!isActive())
		return false;
	glUniform1i(loc, value); GLERR
	return true;
}

inline bool ShaderProgram::setUniform(const char* name, const glm::ivec2& value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return false;
	if (!isActive())
		return false;
	glUniform2iv(loc, 1, glm::value_ptr(value)); GLERR
	return true;
}

inline bool ShaderProgram::setUniform(const char* name, const glm::ivec3& value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return false;
	if (!isActive())
		return false;
	glUniform3iv(loc, 1, glm::value_ptr(value)); GLERR
	return true;
}

inline bool ShaderProgram::setUniform(const char* name, const glm::ivec4& value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return false;
	if (!isActive())
		return false;
	glUniform4iv(loc, 1, glm::value_ptr(value)); GLERR
	return true;
}

inline bool ShaderProgram::setUniform(const char* name, GLuint value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return false;
	if (!isActive())
		return false;
	glUniform1ui(loc, value); GLERR
	return true;
}

inline bool ShaderProgram::setUniform(const char* name, const glm::uvec2& value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return false;
	if (!isActive())
		return false;
	glUniform2uiv(loc, 1, glm::value_ptr(value)); GLERR
	return true;
}

inline bool ShaderProgram::setUniform(const char* name, const glm::uvec3& value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return false;
	if (!isActive())
		return false;
	glUniform3uiv(loc, 1, glm::value_ptr(value)); GLERR
	return true;
}

inline bool ShaderProgram::setUniform(const char* name, const glm::uvec4& value)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return false;
	if (!isActive())
		return false;
	glUniform4uiv(loc, 1, glm::value_ptr(value)); GLERR
	return true;
}

inline bool ShaderProgram::setUniform(const char*name, const glm::mat2& value, bool transpose)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return false;
	if (!isActive())
		return false;
	glUniformMatrix2fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)); GLERR
	return true;
}

inline bool ShaderProgram::setUniform(const char* name, const glm::mat3& value, bool transpose)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return false;
	if (!isActive())
		return false;
	glUniformMatrix3fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)); GLERR
	return true;
}

inline bool ShaderProgram::setUniform(const char* name, const glm::mat4& value, bool transpose)
{
	GLint loc = getUniformLocation(name);
	if (loc == -1)
		return false;
	if (!isActive())
		return false;
	glUniformMatrix4fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value)); GLERR
	return true;
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
	//Texture2DMS,
	TextureCube
};

class RenderTarget
{
public:
	RenderTarget() :
		attachment(GL_INVALID_ENUM),
		rb(),
		type(RenderTargetType::Empty),
		width(0),
		height(0),
		miplevels(0),
		active(false)
	{}
	~RenderTarget()
	{}
	RenderTarget(std::shared_ptr<Texture2D> _tex, GLenum _attachment, GLsizei _width, GLsizei _height, GLsizei _miplevels = 1) :
		tex(_tex),
		attachment(_attachment),
		type(RenderTargetType::Texture2D),
		width(_width),
		height(_height),
		miplevels(_miplevels),
		active(true)
	{}
	RenderTarget(std::shared_ptr<RenderBuffer> _rb, GLenum _attachment, GLsizei _width, GLsizei _height) :
		rb(_rb),
		attachment(_attachment),
		type(RenderTargetType::RenderBuffer),
		width(_width),
		height(_height),
		miplevels(1),
		active(true)
	{}
	RenderTarget(std::shared_ptr<TextureCube> _ctex, GLenum _attachment, GLsizei _width, GLsizei _height, GLsizei _miplevels = 1) :
		ctex(_ctex),
		attachment(_attachment),
		type(RenderTargetType::TextureCube),
		width(_width),
		height(_height),
		miplevels(_miplevels),
		active(true)
	{}
	RenderTarget(const RenderTarget& other) 
	{
		if (other.type == RenderTargetType::Empty)
		{
			type = RenderTargetType::Empty;
			attachment = GL_INVALID_ENUM;
			width = other.width;
			height = other.height;
			miplevels = other.miplevels;
			active = other.active;
		}
		if (other.type == RenderTargetType::Texture2D)
		{
			attachment = other.attachment;
			tex = other.tex;
			type = RenderTargetType::Texture2D;
			width = other.width;
			height = other.height;
			miplevels = other.miplevels;
			active = other.active;
		}
		else if (other.type == RenderTargetType::RenderBuffer)
		{
			attachment = other.attachment;
			rb = other.rb;
			type = RenderTargetType::RenderBuffer;
			width = other.width;
			height = other.height;
			miplevels = other.miplevels;
			active = other.active;
		}
		else if (other.type == RenderTargetType::TextureCube)
		{
			attachment = other.attachment;
			ctex = other.ctex;
			type = RenderTargetType::TextureCube;
			width = other.width;
			height = other.height;
			miplevels = other.miplevels;
			active = other.active;
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
			width = other.width;
			height = other.height;
			miplevels = other.miplevels;
			active = other.active;
		}
		if (other.type == RenderTargetType::Texture2D)
		{
			attachment = other.attachment;
			tex = other.tex;
			type = RenderTargetType::Texture2D;
			width = other.width;
			height = other.height;
			miplevels = other.miplevels;
			active = other.active;
		}
		else if (other.type == RenderTargetType::RenderBuffer)
		{
			attachment = other.attachment;
			rb = other.rb;
			type = RenderTargetType::RenderBuffer;
			width = other.width;
			height = other.height;
			miplevels = other.miplevels;
			active = other.active;
		}
		else if (other.type == RenderTargetType::TextureCube)
		{
			attachment = other.attachment;
			ctex = other.ctex;
			type = RenderTargetType::TextureCube;
			width = other.width;
			height = other.height;
			miplevels = other.miplevels;
			active = other.active;
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
	GLsizei width;
	GLsizei height;
	GLsizei miplevels;
	GLenum attachment;
	bool active;
};

struct RenderTargetSet
{
	std::vector<RenderTarget> colorTargets;
	RenderTarget depthTarget;
};

class FrameBuffer
{
public:
	FrameBuffer() : 
		fbo(0),
		state(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
	{}
	FrameBuffer(GLuint _fbo) :
		fbo(_fbo),
		state(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
	{
		
	}
	~FrameBuffer()
	{
		if (fbo)
			glDeleteFramebuffers(1, &fbo);
	}
	void bind(GLenum target);
	void unbind(GLenum target);
	bool selectColorTargetMipmapLevel(size_t colorTargetIndex , GLint level);
	bool selectDepthTargetMipmapLevel(GLint level);
	bool attachRenderTargetSet(const RenderTargetSet& rtset);
	GLuint fbo;
	/*std::vector<RenderTarget> colorTargets;
	RenderTarget depthTarget;*/
	RenderTargetSet rtset;
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
		samples(0),
		miplevels(0)
	{}

	RenderTargetDesc(GLsizei _width,
					 GLsizei _height,
					 GLenum _internalformat,
					 GLenum _attachment,
					 RenderTargetType _type,
					 int _miplevels = 1,
					 int samples = 0) :
		width(_width),
		height(_height),
		internalformat(_internalformat),
		attachment(_attachment),
		type(_type),
		samples(samples),
		miplevels(_miplevels)
	{}

	~RenderTargetDesc()
	{}

	GLsizei width;
	GLsizei height;
	GLenum internalformat;
	GLenum attachment;
	RenderTargetType type;
	int samples;
	int miplevels;
};

struct FrameBufferDesc
{
	std::vector<RenderTargetDesc> colorTargets;
	RenderTargetDesc depthTarget;
};
#endif