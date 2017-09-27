// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PLUGIN2_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PLUGIN2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include <boost/config.hpp>
#include "IModulCoreTest_API.h"
#include <iostream>
// This class is exported from the Plugin2.dll
class ModulCoreTest : public IModulCoreTest_API {
public:
	ModulCoreTest(void);
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }
	bool startUp();//do stuff?
private:
	ModuleInformation m_info;
	ipengine::Core* core;
	std::vector<ipengine::Scheduler::SubHandle> handles;
	// Inherited via IModulCoreTest_API
	virtual void giveSched(ipengine::Core *) override;
	void writeOutput(ipengine::TaskContext& cont);
	// Inherited via IModulCoreTest_API
	virtual void start() override;
};

extern "C" BOOST_SYMBOL_EXPORT ModulCoreTest module;
ModulCoreTest module;