#ifndef _VulkanRenderer_H_
#define _VulkanRenderer_H_

#include <boost/config.hpp>
#include <IModule_API.h>
#include <glm/glm.hpp>
#include <IGraphics_API.h>
#include "deferred_renderer.h"
#include <ISimpleContentModule_API.h>
class VulkanRenderer : public IGraphics_API {
public:
	VulkanRenderer();
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }

private:
	ModuleInformation m_info;
	DeferredRenderer *m_renderer;
	std::vector<ipengine::Scheduler::SubHandle> handles;
	std::string m_scmID = "SCM";
	boost::shared_ptr<SCM::ISimpleContentModule_API> m_scm;

	//Maps to map vulkan <-> scm resource id's
	//std::unordered_map<ipengine::ipid, std::shared_ptr<VAO>> m_scmmeshtovao;
	//std::unordered_map<ipengine::ipid, std::shared_ptr<ShaderProgram>> m_scmshadertoprogram;
	std::unordered_map <ipengine::ipid, rj::ImageWrapper> m_scmtexturetot2d;
	//SCM entity that represents the rendering camera
	ipengine::ipid m_entrepcam;
	//Update vulkan resources from SCM resources
	void updateData();

	// Inherited via IGraphics_API
	virtual bool _startup() override;

	void render(ipengine::TaskContext & c);

	virtual bool _shutdown() override;

	virtual void render() override;

	virtual void render(int fbo, int viewportx, int viewporty, bool multisample) override;

	virtual void setCameraEntity(ipengine::ipid) override;

	virtual void setFOV(uint32_t) override;

	virtual void setResolution(uint32_t, uint32_t) override;

	virtual void setClipRange(float, float) override;

	virtual ipengine::ipid getCameraEntity() override;

	virtual uint32_t getFOV() override;

	virtual void getResolution(uint32_t &, uint32_t &) override;

	virtual void getClipRange(float &, float &) override;

	virtual void loadTextureFromMemory(const GrAPI::t2d &, const ipengine::ipid) override;

	//helpers ------------------------------------------------------------------------------------------------
	std::vector<ipengine::ipid> getActiveEntityNames(SCM::ISimpleContentModule_API & scm);
	glm::mat4 parentInfluencedTransform(ipengine::ipid childid);
	glm::mat4 parentInfluencedView(ipengine::ipid childid);
	std::vector<Vertex> scmVertsToVVertex(SCM::VertexVector&);

};

extern "C" BOOST_SYMBOL_EXPORT VulkanRenderer module;
VulkanRenderer module;

#endif