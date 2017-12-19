#include <IPCore/Core/ICore.h>

ipengine::Core::Core() :
	cmodule_console(std::cout),
	cmodule_scheduler(),
	cmodule_threadingservices(std::thread::hardware_concurrency() - 5),
	cmodule_endpointregistry(),
	core_idgen(1)
{
}

ipengine::Core::~Core()
{
}

void ipengine::Core::initialize()
{
	core_msgep = cmodule_endpointregistry.createEndpoint("CORE");
	cmodule_threadingservices.startWorkers();
}

void ipengine::Core::shutdown()
{
	cmodule_threadingservices.stopWorkers();
}

ipengine::Time ipengine::Core::tick(bool& shouldstop)
{
	auto t1 = Time::now();

	cmodule_scheduler.schedule();
	core_msgep->sendPendingMessages();
	core_msgep->dispatch();

	auto t2 = Time::now();
	return Time(t2.nano() - t1.nano());
}

ipengine::ipid ipengine::Core::createID()
{
	return core_idgen.fetch_add(1, std::memory_order_relaxed);
}

ipengine::Scheduler & ipengine::Core::getScheduler()
{
	return cmodule_scheduler;
}

ipengine::Console & ipengine::Core::getConsole()
{
	return cmodule_console;
}

ipengine::ThreadPool & ipengine::Core::getThreadPool()
{
	return cmodule_threadingservices;
}

ipengine::EndpointRegistry & ipengine::Core::getEndpointRegistry()
{
	return cmodule_endpointregistry;
}
