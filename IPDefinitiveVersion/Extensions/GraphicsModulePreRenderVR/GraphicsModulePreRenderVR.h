#ifndef _ExtensionTest_H_
#define _ExtensionTest_H_

#include <boost/config.hpp>
#include <IModule_API.h>
#include <ISimpleContentModule_API.h>
#include <IBasicOpenVRModule_API.h>
#include <IGraphics_API.h>
#include <glm/glm.hpp>
#include <glew\include\GL\glew.h>
#include <IDataStoreModuleh_API.h>
class GraphicsModulePreRenderVR : public IExtensionPoint {
public:
	GraphicsModulePreRenderVR();
	// TODO: add your methods here.
	virtual void execute(std::vector<std::string> argnames, std::vector<ipengine::any>& args) override;


	// Inherited via IExtensionPoint
	virtual ExtensionInformation * getInfo() override;

	struct FramebufferDesc
	{
		GLuint m_nDepthBufferId;
		GLuint m_nRenderTextureId;
		GLuint m_nRenderFramebufferId;
		GLuint m_nResolveTextureId;
		GLuint m_nResolveFramebufferId;
	};

	uint32_t renderHeight, renderWidth;

	GLuint quadVAO, quadVBO;
	boost::shared_ptr<SCM::ISimpleContentModule_API> scm;
	FramebufferDesc leftEyeDesc;
	FramebufferDesc rightEyeDesc;
	vr::TrackedDevicePose_t lastposes[vr::k_unMaxTrackedDeviceCount];
private:

	static const GLfloat QuadVerts[];

	bool CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc &framebufferDesc)
	{
		//create multisample framebuffer
		glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);

		glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);

		glGenTextures(1, &framebufferDesc.m_nRenderTextureId);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, true);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);

		//create resolve framebuffer as intermediate to get a normal 2d texture from multisampled buffer for submitting to the HMD and rendering onto window quad
		glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId);

		glGenTextures(1, &framebufferDesc.m_nResolveTextureId);
		glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0);

		// check FBO status
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			return false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return true;
	}


	void resolveFB(GLint read, GLint draw, glm::uint32 renderwidth, glm::uint32 renderheight)
	{
		glDisable(GL_MULTISAMPLE);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, read);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw);
		glBlitFramebuffer(0, 0, renderwidth, renderheight, 0, 0, renderwidth, renderheight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}
	glm::mat4 convert(const vr::HmdMatrix44_t &m) {
		return glm::mat4(
			m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0],
			m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1],
			m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2],
			m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]);
	}

	glm::mat4x3 convert(const vr::HmdMatrix34_t &m) {
		return glm::mat4x3(
			m.m[0][0], m.m[1][0], m.m[2][0],
			m.m[0][1], m.m[1][1], m.m[2][1],
			m.m[0][2], m.m[1][2], m.m[2][2],
			m.m[0][3], m.m[1][3], m.m[2][3]);
	}

	boost::shared_ptr<IBasicOpenVRModule_API> ovrmodule;
	boost::shared_ptr<IDataStoreModuleh_API> datastore;
	ExtensionInformation m_info;
	std::vector<ipengine::Scheduler::SubHandle> handle;
	bool init = false;
};

extern "C" BOOST_SYMBOL_EXPORT GraphicsModulePreRenderVR extension;
GraphicsModulePreRenderVR extension;

#endif