// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PLUGIN1_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PLUGIN1_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#include "IInput_API.h"
#include <boost/config.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <deque>
#include <map>
#include <chrono>
#include <thread>
#include <mutex>
// This class is exported from the Plugin1.dll
class InputModule : public IInput_API {
public:
	InputModule(void);
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }
	bool startUp();// { /*printStuffToSomething(m_info.identifier + " successfully started up as " + m_info.iam); */return true; } //do stuff?
	const std::vector<IInput::Input> getInputBuffered(int millisecondsIntoThePast);
	//void printStuffToSomething(std::string text) { std::cout << text << std::endl; }
private:
	ModuleInformation m_info;
	SDL_Window* window;
	std::thread memes;
	std::chrono::high_resolution_clock::duration timeToRetainInput = std::chrono::milliseconds(2000);
	std::chrono::high_resolution_clock clock;
	std::multimap<time_t, IInput::Input> inputData;
	bool isManipulating = false;
	std::mutex mymutex;
	void pollData();
};
extern "C" BOOST_SYMBOL_EXPORT InputModule module;
InputModule module;