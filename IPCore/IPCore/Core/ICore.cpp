#include <Core/ICore.h>

ipengine::Core::Core() :
	cmodule_console(std::cout),
	cmodule_scheduler(),
	cmodule_threadingservices(0)//std::thread::hardware_concurrency() - 1)
{
}

ipengine::Core::~Core()
{
}

void ipengine::Core::initialize()
{

}

void ipengine::Core::shutdown()
{

}

ipengine::Scheduler & ipengine::Core::getScheduler()
{
	return cmodule_scheduler;
}

ipengine::Console & ipengine::Core::getConsole()
{
	// TODO: hier Rückgabeanweisung eingeben
	return cmodule_console;
}

ipengine::ThreadPool & ipengine::Core::getThreadPool()
{
	return cmodule_threadingservices;
}
