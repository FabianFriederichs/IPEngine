#include "Application.h"

#include <Injector/Injector.h>
#include <IPCore/Core/ICore.h>
#include <iostream>
//implementation
class ipengine::Application::ApplicationImpl
{
private:
	friend class ipengine::Application;

	ApplicationImpl() :
		core(nullptr),
		inj(nullptr),
		shouldStopFlag(false)
	{

	}

	~ApplicationImpl()
	{
		if(inj)
			delete inj;
		if(core)
			delete core;
	}

	void init(const iprstr _configPath)
	{
		core = new ipengine::Core();
		core->initialize(_configPath);

		inj = new Injector(core->getConfigManager().getString("core.injector.depgraph_path"),
						   core->getConfigManager().getString("core.injector.module_path"));
		inj->LoadModules(core);		
	}

	void shutdown()
	{
		inj->shutdown();
		core->shutdown();
	}

	void consoleThreadFunc(Application& app)
	{
		auto sleepinterval = core->getConfigManager().getInt("core.application.console_thread_sleep_interval");
		std::string cmd;
		while (!shouldStopFlag.load(std::memory_order_acquire))
		{
			app.onConsole();
			std::getline(std::cin, cmd);
			getCore().getConsole().in(cmd.c_str());
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepinterval));
		}
	}

	void run(Application& app)
	{
		core->run();
		app.initialize();

		bool enableConsole = core->getConfigManager().getBool("core.application.enable_native_console");
		std::thread consoleThread;
		if(enableConsole)
			consoleThread = std::move(std::thread(&ApplicationImpl::consoleThreadFunc, this, std::ref(app)));

		bool shouldstop = false;
		while (!shouldstop)
		{
			app.preTick();
			ipengine::Time t = core->tick(shouldstop);
			app.postTick(t);
		}

		shouldStopFlag.store(true, std::memory_order_release);
		if (enableConsole)
		{
			//consoleThread.join();
			consoleThread.detach();
		}

		shutdown();
		app.onShutdown();
	}

	Injector& getInjector()
	{
		return *inj;
	}

	Core& getCore()
	{
		return *core;
	}

private:
	Injector* inj;
	ipengine::Core* core;
	std::atomic_bool shouldStopFlag;
};

ipengine::Application::Application() :
	m_pimpl(new ApplicationImpl())
{}

ipengine::Application::~Application()
{
	if (m_pimpl)
		delete m_pimpl;
}

ipengine::Application::Application(Application && app) :
	m_pimpl(app.m_pimpl)
{
	app.m_pimpl = nullptr;
}

ipengine::Application & ipengine::Application::operator=(Application && app)
{
	if (this == &app)
		return *this;
	if (m_pimpl != nullptr)
		delete m_pimpl;
	m_pimpl = app.m_pimpl;
	app.m_pimpl = nullptr;
	return *this;
}

void ipengine::Application::init(const iprstr configPath)
{
	m_pimpl->init(configPath);
}

void ipengine::Application::run()
{
	m_pimpl->run(*this);
}

Injector& ipengine::Application::getInjector()
{
	return m_pimpl->getInjector();
}

ipengine::Core & ipengine::Application::getCore()
{
	return m_pimpl->getCore();
}
