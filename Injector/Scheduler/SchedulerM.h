// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PLUGIN2_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PLUGIN2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include <boost/config.hpp>
#include "IScheduler_API.h"
// This class is exported from the Plugin2.dll
class SchedulerM : public IScheduler_API {
public:
	SchedulerM(void);
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }
	virtual void schedule() override;
	virtual int subscribe(const function<void(TaskContext&)>& schedfunc, Scheduler::interval_t desiredInterval, Scheduler::SubType type, float timescale) override;
	bool startUp() { m_sched.start(); return true;/*	m_info.dependencies.getDep<IPrinter_API>("printer")->printStuffToSomething(m_info.identifier + " successfully started up as " + m_info.iam); return true; */ } //do stuff?
private:
	ModuleInformation m_info;
	Scheduler m_sched;
	std::vector<Scheduler::SubHandle> handles;
	// Inherited via IScheduler_API
	
};

extern "C" BOOST_SYMBOL_EXPORT SchedulerM module;
SchedulerM module;