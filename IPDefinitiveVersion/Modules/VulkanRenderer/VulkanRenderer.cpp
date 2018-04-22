// Plugin2.cpp : Defines the exported functions for the DLL application.
//
#define USE_GLTF
#define TINYGLTF_LOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX

// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED
#include "VulkanRenderer.h"
#ifdef USE_GLTF
std::string GLTF_VERSION;
std::string GLTF_NAME;
#endif

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
VulkanRenderer::VulkanRenderer()
{
	m_info.identifier = "VulkanRenderer";
	m_info.version = "1.0";
	m_info.iam = "IModule_API.IGraphics_API"; //Change this to your used API
	return;
}

bool VulkanRenderer::_startup()
{
#ifdef USE_GLTF
	GLTF_VERSION = "2.0";
	GLTF_NAME = "..//assets//gltf//avocado//avocado.gltf";
#endif
	m_renderer = new DeferredRenderer();
	m_renderer->initialize();

	ipengine::Scheduler& sched = m_core->getScheduler();
	handles.push_back(sched.subscribe(ipengine::TaskFunction::make_func<VulkanRenderer, &VulkanRenderer::render>(this),
		0,
		ipengine::Scheduler::SubType::Frame,
		1,
		&m_core->getThreadPool(),
		true)
	);
	//m_renderer->run();
	return true;
}

void VulkanRenderer::render(ipengine::TaskContext & c)
{
	render();
}

bool VulkanRenderer::_shutdown()
{
	m_renderer->shutdown();
	return true;
}

void VulkanRenderer::render()
{
	m_renderer->render();
}

void VulkanRenderer::render(int fbo, int viewportx, int viewporty, bool multisample)
{
	//set stuff
	//...
	m_renderer->render();
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
