#include "Application.h"

#include <Injector/Injector.h>
#include <IPCore/Core/ICore.h>
//implementation
class ipengine::Application::ApplicationImpl
{
private:
	friend class ipengine::Application;

	ApplicationImpl() :
		core(nullptr),
		inj(nullptr)
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
		core->shutdown();
	}

	void run(Application& app)
	{
		core->run();
		app.initialize();
		bool shouldstop = false;
		while (!shouldstop)
		{
			app.preTick();
			ipengine::Time t = core->tick(shouldstop);
			app.postTick(t);
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
