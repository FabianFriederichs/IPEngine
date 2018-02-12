#ifndef _GRAPHICSMODULE_H_
#define _GRAPHICSMODULE_H_
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PLUGIN2_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PLUGIN2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include <boost/config.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <unordered_map>
#include "libheaders.h"
#include "Utils.h"
#include <IGraphics_API.h>
//#define GLEW_STATIC
// This class is exported from the Plugin2.dll
class GraphicsModule : public IGraphics_API {
public:
	GraphicsModule(void);
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }
	bool startUp();// {/*	m_info.dependencies.getDep<IPrinter_API>("printer")->printStuffToSomething(m_info.identifier + " successfully started up as " + m_info.iam); */return true;  } //do stuff?
	void render();
	void render(ipengine::TaskContext& c);
	virtual void setCameraEntity(ipengine::ipid v) override;
	virtual void setFOV(uint32_t v) override;
	virtual void setResolution(uint32_t x, uint32_t y) override;
	virtual void setClipRange(float n,float f) override;
	virtual ipengine::ipid getCameraEntity() override;
	virtual uint32_t getFOV() override;
	virtual void getResolution(uint32_t &, uint32_t &) override;
	virtual void getClipRange(float &, float&) override;
	
	

private:
	std::vector<ipengine::ipid> getActiveEntityNames(SCM::ISimpleContentModule_API&);
	ModuleInformation m_info;
	std::string DataDepName;
	SDL_Window* window;
	SDL_GLContext context;
	HGLRC wincontext;
	SDL_SysWMinfo info;
	std::string m_scmID = "SCM";
	boost::shared_ptr<SCM::ISimpleContentModule_API> m_scm;
	std::vector<ipengine::Scheduler::SubHandle> handles;
	ipengine::ipid cameraentity = IPID_INVALID;
	glm::vec4 m_clearcolor = { 0.20f, 0.15f, 0.18f, 1.0f };
	float width = 1280; float height = 720; float znear = 0.1f; float zfar = 100;
	float m_fov = glm::pi<float>() / 2;
	glm::vec3 camerapos = glm::vec3(3, 3, 20);
	glm::mat4 projmat = glm::perspective(m_fov, width / height, znear, zfar);
	glm::mat4 viewmat = glm::mat4(glm::quat(1.0f, 0.0f, .0f, .0f))*translate(glm::mat4(1.0f), -camerapos);


	void setupSDL();
	void loadShaders();
	void updateData();
	std::unordered_map<ipengine::ipid, std::shared_ptr<VAO>> m_scmmeshtovao;
	std::unordered_map<ipengine::ipid, std::shared_ptr<ShaderProgram>> m_scmshadertoprogram;
	std::unordered_map <ipengine::ipid, std::shared_ptr<Texture2D>> m_scmtexturetot2d;
	void drawSCMMesh(ipengine::ipid);

	// Inherited via IGraphics_API
	glm::mat4 ViewFromTransData(const SCM::TransformData*);
	void recalcProj();

	// Inherited via IGraphics_API

	//container with vao to scm mesh id

	//rendering helpers
	void drawEntities();
};

extern "C" BOOST_SYMBOL_EXPORT GraphicsModule module;
GraphicsModule module;

#endif