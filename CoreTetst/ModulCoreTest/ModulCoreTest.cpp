// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "ModulCoreTest.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
ModulCoreTest::ModulCoreTest()
{
	m_info.identifier = "ModulCoreTest";
	m_info.version = "1.0";
	m_info.iam = "IModulCoreTest_API";
	return;
}

bool ModulCoreTest::startUp()
{
	return true;
}

void ModulCoreTest::giveSched(ipengine::Core * newsched)
{
	core = newsched;
}

void ModulCoreTest::writeOutput(ipengine::TaskContext& cont)
{
	ipengine::Scheduler::SchedInfo info = cont;
	static auto t = std::chrono::system_clock::now();
	std::cout << "Irgendwas Funktioniert mit Delta: " << info.dt << std::endl;// std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now() - t).count() << std::endl;
	t = std::chrono::system_clock::now();
}

void ModulCoreTest::start()
{
	ipengine::Scheduler& sched = core->getScheduler();
	core->getThreadPool().startWorkers();
	handles.push_back(sched.subscribe(ipengine::TaskFunction::make_func<ModulCoreTest, &ModulCoreTest::writeOutput>(this), 0, ipengine::Scheduler::SubType::Frame, 1, &core->getThreadPool()));

}
