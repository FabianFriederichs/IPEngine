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
	setupCoreConsoleCommands();
	m_isrunning.store(false);
}

void ipengine::Core::run()
{
	cmodule_threadingservices->startWorkers();
	m_isrunning.store(true);
}

void ipengine::Core::shutdown()
{
	if (!m_isrunning.load())
	{
		//TODO: shutdown stuff
	}
}

void ipengine::Core::stop()
{	
	cmodule_threadingservices->stopWorkers();
	m_isrunning.store(false);
}

ipengine::Time ipengine::Core::tick(bool& shouldstop)
{
	auto t1 = Time::now();

	if (m_isrunning.load(std::memory_order_relaxed))
	{
		cmodule_scheduler->schedule();
		core_msgep->sendPendingMessages();
		core_msgep->dispatch();
	}

	if (!m_isrunning.load(std::memory_order_relaxed))
		shouldstop = true;

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

void ipengine::Core::setupCoreConsoleCommands()
{
	cmodule_console->addCommand("core.shutdown", CommandFunc::make_func<Core, &Core::cmd_shutdown>(this), "Stops scheduling and all workers.");
	cmodule_console->addCommand("help", CommandFunc::make_func<Core, &Core::cmd_listcommands>(this), "Prints a list of available console commands.");
}

void ipengine::Core::cmd_shutdown(const ConsoleParams & params)
{
	if (params.getParamCount() == 1)
		cmodule_console->println(params.get(0));
	stop();
}

void ipengine::Core::cmd_listcommands(const ConsoleParams & params)
{
	cmodule_console->listCommands();
}
