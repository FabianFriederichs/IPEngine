// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PLUGIN1_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PLUGIN1_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef PLUGIN1_EXPORTS
#define PLUGIN1_API __declspec(dllexport)
#else
#define PLUGIN1_API __declspec(dllimport)
#endif
#include "IPrinter_API.h"
#include <iostream>
#include <boost/config.hpp>
// This class is exported from the Plugin1.dll
class PLUGIN1_API stdOutPrinter : public IPrinter {
public:
	stdOutPrinter(void);
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }
	void printStuffToSomething(std::string text) { std::cout << text << std::endl; }
private:
	ModuleInformation m_info;
};
extern "C" BOOST_SYMBOL_EXPORT stdOutPrinter module;
stdOutPrinter module;