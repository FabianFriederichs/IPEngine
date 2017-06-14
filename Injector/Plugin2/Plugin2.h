// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PLUGIN2_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PLUGIN2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef PLUGIN2_EXPORTS
#define PLUGIN2_API __declspec(dllexport)
#else
#define PLUGIN2_API __declspec(dllimport)
#endif
#include <boost/config.hpp>
#include "IModule_API.h"
#include "IPrinter_API.h"
// This class is exported from the Plugin2.dll
class CPlugin2 : public IModule_API {
public:
	CPlugin2(void);
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }
	bool startUp(){	m_info.dependencies.getDep<IPrinter_API>("printer")->printStuffToSomething(m_info.identifier + " successfully started up as " + m_info.iam); return true; } //do stuff?
private:
	ModuleInformation m_info;
};

extern "C" BOOST_SYMBOL_EXPORT CPlugin2 module;
CPlugin2 module;