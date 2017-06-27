// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PLUGIN2_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PLUGIN2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include <boost/config.hpp>
#include "IGraphics_API.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <memory>
#include "libheaders.h"
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
	ModuleInformation m_info;
	std::string DataDepName;
	SDL_Window* window;
	void setupSDL();
	void loadShaders();
	void updateData();
};

extern "C" BOOST_SYMBOL_EXPORT GraphicsModule module;
GraphicsModule module;