// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "SDLWindowManager.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
SDLWindowManager::SDLWindowManager()
{
	m_info.identifier = "SDLWindowManager";
	m_info.version = "1.0";
	m_info.iam = "IModule_API"; //Change this to your used API
	return;
}


bool SDLWindowManager::_startup()
{
	auto res = SDL_Init(SDL_INIT_VIDEO);
	if (res != 0)
	{
		//std::cout << "Could not initialize SDL." << std::endl;
		return false;
		throw std::runtime_error("failed to create window surface!");
	}

	/*SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, static_cast<int>(m_core->getConfigManager().getInt("graphics.opengl.version_major")));
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, static_cast<int>(m_core->getConfigManager().getInt("graphics.opengl.version_minor")));
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	if (m_core->getConfigManager().getBool("graphics.window.msaa"))
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, static_cast<int>(m_core->getConfigManager().getInt("graphics.window.msaa_samples")));
	}
*/
	title = m_core->getConfigManager().getString("windowmanager.window.windowtitle");
	m_width = m_core->getConfigManager().getInt("windowmanager.window.width");
	m_height = m_core->getConfigManager().getInt("windowmanager.window.height");
	
	return true;
}

bool SDLWindowManager::_shutdown()
{
	for (auto w : m_windows)
	{
		SDL_DestroyWindow(w);
	}
	return true;
}

int SDLWindowManager::getHeight()
{
	return m_height;
}

int SDLWindowManager::getWidth()
{
	return m_width;
}


SDL_SysWMinfo SDLWindowManager::getWindowInfo(SDL_Window *w)
{
	SDL_SysWMinfo winfo;
	SDL_VERSION(&winfo.version);
	auto res = SDL_GetWindowWMInfo(w, &winfo);
	return winfo;
}

SDL_Window * SDLWindowManager::getNewWindow()
{
	auto m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, static_cast<int>(m_width), static_cast<int>(m_height), SDL_WINDOW_OPENGL);
	auto err = SDL_GetError();
	std::string error(err);
	if (m_window == NULL)
	{
		throw std::runtime_error("failed to create window surface!");
	}
	SDL_SysWMinfo winfo;
	SDL_VERSION(&winfo.version);
	auto res = SDL_GetWindowWMInfo(m_window, &winfo);
	err = SDL_GetError();

	if (res != SDL_TRUE)
	{
		//std::cout << "Could not initialize SDL." << std::endl;
		return false;
		throw std::runtime_error("failed to create window surface!");
	}
	m_windows.push_back(m_window);
	return m_window;
}

void SDLWindowManager::destroy(SDL_Window * w)
{
	SDL_DestroyWindow(w);
	int index = 0;
	for (auto wi : m_windows)
	{
		if (wi == w)
			m_windows.erase(m_windows.begin()+index);
		++index;
	}
}
