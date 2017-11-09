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
#include <memory>
#include <unordered_map>
#include "libheaders.h"
#include "Utils.h"
#include "IGraphics_API.h"
//#define GLEW_STATIC
// This class is exported from the Plugin2.dll
class GraphicsModule : public IGraphics_API {
public:
	GraphicsModule(void);
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }
	bool startUp();// {/*	m_info.dependencies.getDep<IPrinter_API>("printer")->printStuffToSomething(m_info.identifier + " successfully started up as " + m_info.iam); */return true;  } //do stuff?
	void render();
private:
	std::vector<std::string> getActiveEntityNames(SCM::ISimpleContentModule_API&);
	ModuleInformation m_info;
	std::string DataDepName;
	SDL_Window* window;
	std::string m_scmID = "scm";
	boost::shared_ptr<SCM::ISimpleContentModule_API> m_scm;

	//constants
	glm::vec4 m_clearcolor = { 0.15f, 0.15f, 0.18f, 1.0f };
	float width = 1280; float height = 720; float znear = 0.1f; float zfar = 100;
	float m_fov = glm::pi<float>() / 2;
	//

	void setupSDL();
	void loadShaders();
	void updateData();
	std::unordered_map<SCM::IdType, std::shared_ptr<VAO>> m_scmmeshtovao;
	std::unordered_map<SCM::IdType, std::shared_ptr<ShaderProgram>> m_scmshadertoprogram;

	void drawSCMMesh(SCM::IdType);
	//container with vao to scm mesh id
};

extern "C" BOOST_SYMBOL_EXPORT GraphicsModule module;
GraphicsModule module;

#endif