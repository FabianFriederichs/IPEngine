// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "VulkanRenderer.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
VulkanRenderer::VulkanRenderer()
{
	m_info.identifier = "VulkanRenderer";
	m_info.version = "1.0";
	m_info.iam = "IModule_API.IGraphics_API"; //Change this to your used API
	return;
}


bool VulkanRenderer::startUp()
{
	//Initialize your module
	return false;
}

bool VulkanRenderer::_startup()
{
	m_renderer = std::make_unique<DeferredRenderer>();
	return false;
}

bool VulkanRenderer::_shutdown()
{
	return false;
}

void VulkanRenderer::render()
{
}

void VulkanRenderer::render(int fbo, int viewportx, int viewporty, bool multisample)
{
}

void VulkanRenderer::setCameraEntity(ipengine::ipid)
{
}

void VulkanRenderer::setFOV(uint32_t)
{
}

void VulkanRenderer::setResolution(uint32_t, uint32_t)
{
}

void VulkanRenderer::setClipRange(float, float)
{
}

ipengine::ipid VulkanRenderer::getCameraEntity()
{
	return ipengine::ipid();
}

uint32_t VulkanRenderer::getFOV()
{
	return uint32_t();
}

void VulkanRenderer::getResolution(uint32_t &, uint32_t &)
{
}

void VulkanRenderer::getClipRange(float &, float &)
{
}

void VulkanRenderer::loadTextureFromMemory(const GrAPI::t2d &, const ipengine::ipid)
{
}
