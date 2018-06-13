#ifndef _IWindowManager_API_H_
#define _IWindowManager_API_H_

#include <IModule_API.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

class IWindowManager_API : public IModule_API {
public:
	virtual SDL_SysWMinfo getWindowInfo(SDL_Window*) = 0;
	virtual SDL_Window * getNewWindow() = 0;
	virtual int getHeight() = 0;
	virtual int getWidth() = 0;
	virtual void destroy(SDL_Window*) = 0;
};

#endif
