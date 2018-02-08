#include <IPCore/Core/ICore.h>

ipengine::Core::Core() :
	cmodule_console(nullptr),
	cmodule_scheduler(nullptr),
	cmodule_threadingservices(nullptr),
	cmodule_endpointregistry(nullptr),
	core_idgen(1)
{
}

ipengine::Core::~Core()
{
}

void ipengine::Core::initialize(const iprstr configpath)
{
	cmodule_memorymanager = new MemoryManager();
	cmodule_configmanager = new ConfigManager();
	if (!cmodule_configmanager->loadConfigFile(configpath))
	{
		//report error somehow (debug interface!)
	}

	cmodule_console = new Console(std::cout);
	cmodule_scheduler = new Scheduler();	
	cmodule_threadingservices = new ThreadPool(cmodule_configmanager->getInt("core.threading.max_worker_threads"));
	cmodule_endpointregistry = new EndpointRegistry();
	core_msgep = cmodule_endpointregistry->createEndpoint("CORE");	
}

void ipengine::Core::run()
{
	cmodule_threadingservices->startWorkers();
}

void ipengine::Core::shutdown()
{
	cmodule_threadingservices->stopWorkers();
}

ipengine::Time ipengine::Core::tick(bool& shouldstop)
{
	auto t1 = Time::now();

	cmodule_scheduler->schedule();
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
	return *cmodule_scheduler;
}

ipengine::Console & ipengine::Core::getConsole()
{
	return *cmodule_console;
}

ipengine::ThreadPool & ipengine::Core::getThreadPool()
{
	return *cmodule_threadingservices;
}

ipengine::EndpointRegistry & ipengine::Core::getEndpointRegistry()
{
	return *cmodule_endpointregistry;
}

ipengine::MemoryManager & ipengine::Core::getMemoryManager()
{
	return *cmodule_memorymanager;
}

ipengine::ConfigManager & ipengine::Core::getConfigManager()
{
	return *cmodule_configmanager;
}
