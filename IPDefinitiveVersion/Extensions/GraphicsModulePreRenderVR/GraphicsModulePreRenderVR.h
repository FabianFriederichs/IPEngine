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
#include <ISimpleSceneModule_API.h>
#include "glerror.h"
class GraphicsModulePreRenderVR : public IExtension {
public:
	GraphicsModulePreRenderVR();
	// TODO: add your methods here.
	virtual void execute(std::vector<std::string> argnames, std::vector<ipengine::any>& args) override;


	// Inherited via IExtension
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
	float controllersphereradius = 0.05f;
private:

	static const GLfloat QuadVerts[];

	bool CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc &framebufferDesc)
	{
		int mssamples = static_cast<int>(m_core->getConfigManager().getInt("graphics.vr.msaa_samples"));
		//mssamples = mssamples != 0 ? mssamples : 4;
		//create multisample framebuffer
		glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId); GLERR
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId); GLERR

		glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId); GLERR
		glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId); GLERR
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, mssamples, GL_DEPTH_COMPONENT, nWidth, nHeight); GLERR
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId); GLERR

		glGenTextures(1, &framebufferDesc.m_nRenderTextureId); GLERR
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId); GLERR
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, mssamples, GL_RGBA8, nWidth, nHeight, true); GLERR
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0); GLERR

		//create resolve framebuffer as intermediate to get a normal 2d texture from multisampled buffer for submitting to the HMD and rendering onto window quad
		glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId); GLERR
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId); GLERR

		glGenTextures(1, &framebufferDesc.m_nResolveTextureId); GLERR
		glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId); GLERR
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); GLERR
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0); GLERR
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr); GLERR
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0); GLERR

		// check FBO status
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			return false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0); GLERR

		return true;
	}


	void resolveFB(GLint read, GLint draw, glm::uint32 renderwidth, glm::uint32 renderheight)
	{
		//glDisable(GL_MULTISAMPLE);
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
	boost::shared_ptr<ISimpleSceneModule_API> scenemodule;
	ExtensionInformation m_info;
	std::vector<ipengine::Scheduler::SubHandle> handle;
	bool init = false;

	ipengine::ipid hmdid = IPID_INVALID, lctrlid = IPID_INVALID, rctrlid = IPID_INVALID, cameraid = IPID_INVALID;
};

extern "C" BOOST_SYMBOL_EXPORT GraphicsModulePreRenderVR extension;
GraphicsModulePreRenderVR extension;

#endif