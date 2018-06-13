#ifndef _VulkanRenderer_H_
#define _VulkanRenderer_H_

#include <boost/config.hpp>
#include <IModule_API.h>
#include <IGraphics_API.h>
#include "deferred_renderer.h"

class VulkanRenderer : public IGraphics_API {
public:
	VulkanRenderer();
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }
	bool startUp();
private:
	ModuleInformation m_info;
	std::unique_ptr<DeferredRenderer> m_renderer;

	// Inherited via IGraphics_API
	virtual bool _startup() override;

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

};

extern "C" BOOST_SYMBOL_EXPORT VulkanRenderer module;
VulkanRenderer module;

#endif