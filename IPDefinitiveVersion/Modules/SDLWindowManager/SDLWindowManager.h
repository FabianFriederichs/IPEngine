#ifndef _SDLWindowManager_H_
#define _SDLWindowManager_H_

#include <boost/config.hpp>
#include <IWindowManager_API.h>

class SDLWindowManager : public IWindowManager_API {
public:
	SDLWindowManager();
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }

	// Inherited via IWindowManager_API
	virtual SDL_SysWMinfo getWindowInfo(SDL_Window *) override;
	virtual SDL_Window * getNewWindow() override;
	virtual void destroy(SDL_Window *) override;
	// Inherited via IWindowManager_API
	virtual int getHeight() override;
	virtual int getWidth() override;
private:
	ModuleInformation m_info;
	std::vector<SDL_Window*> m_windows;
	int m_width, m_height;
	std::string title;
	// Inherited via IWindowManager_API
	virtual bool _startup() override;

	virtual bool _shutdown() override;

	

	
};

extern "C" BOOST_SYMBOL_EXPORT SDLWindowManager module;
SDLWindowManager module;

#endif