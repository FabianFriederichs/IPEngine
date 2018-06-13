#include <IPCore/Core/ICore.h>
#include <sstream>

ipengine::Core::Core() :
	cmodule_console(nullptr),
	cmodule_scheduler(nullptr),
	cmodule_threadingservices(nullptr),
	cmodule_endpointregistry(nullptr),
	core_idgen()
{
}

ipengine::Core::~Core()
{
}

void ipengine::Core::initialize(const iprstr configpath)
{
	cmodule_memorymanager = new MemoryManager();
	cmodule_configmanager = new ConfigManager();
	cmodule_errormanager = new ErrorManager();
	cmodule_errormanager->registerHandlerFunc(ErrorHandlerFunc::make_func<Core, &Core::handleError>(this));
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

	cmodule_errormanager->handlePendingExceptions();

	cmodule_console->executePendingCommands();

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

void ipengine::Core::handleError(ipex & ex)
{
	std::stringstream msg;
	msg << "Exception raised at '" << ex.getRaiseLocation() << "': " << ex.getMessage() << "\nSeverity: ";

	//TODO: implement logging
	switch (ex.getSeverity())
	{
		case ipex_severity::info:
			msg << "INFO";
			cmodule_console->println(msg.rdbuf()->str().c_str());
			break;
		case ipex_severity::warning:
			msg << "WARNING";
			cmodule_console->println(msg.rdbuf()->str().c_str());
			break;
		case ipex_severity::error:
			msg << "ERROR";
			cmodule_console->println(msg.rdbuf()->str().c_str());
			break;
		case ipex_severity::fatal:
			msg << "FATAL ERROR";
			cmodule_console->println(msg.rdbuf()->str().c_str());
			stop();
			break;
		default:
			break;
	}
}

ipengine::ipid ipengine::Core::createID()
{
	return core_idgen.createID();
}

ipengine::IdGen & ipengine::Core::getIDGen()
{
	return core_idgen;
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

ipengine::ErrorManager & ipengine::Core::getErrorManager()
{
	return *cmodule_errormanager;
}

void ipengine::Core::setupCoreConsoleCommands()
{
	cmodule_console->addCommand("core.shutdown", CommandFunc::make_func<Core, &Core::cmd_shutdown>(this), "Stops scheduling and all workers.");
	cmodule_console->addCommand("help", CommandFunc::make_func<Core, &Core::cmd_listcommands>(this), "Prints a list of available console commands.\nhelp \"<commandname>\" shows detailed help for that command.");
	cmodule_console->addCommand("helpdetailed", CommandFunc::make_func<Core, &Core::cmd_listcommandsd>(this), "Prints a detailed list of available console commands.");
}

void ipengine::Core::cmd_shutdown(const ConsoleParams & params)
{
	if (params.getParamCount() == 1)
		cmodule_console->println(params.get(0));
	stop();
}

void ipengine::Core::cmd_listcommands(const ConsoleParams & params)
{
	if (params.getParamCount() == 1)
	{
		cmodule_console->showCommandDetail(params.get(0));
	}
	else
	{
		cmodule_console->listCommands();
	}
}

void ipengine::Core::cmd_listcommandsd(const ConsoleParams & params)
{
	cmodule_console->listCommandsDetailed();
}