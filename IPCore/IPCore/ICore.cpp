#include "ICore.h"

ipengine::Core::Core() :
	cmodule_console(std::cout)
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

Scheduler & ipengine::Core::getScheduler()
{
	return cmodule_scheduler;
}

ipengine::Console & ipengine::Core::getConsole()
{
	// TODO: hier Rückgabeanweisung eingeben
	return cmodule_console;
}

ThreadPool & ipengine::Core::getThreadPool()
{
	return cmodule_threadingservices;
}
