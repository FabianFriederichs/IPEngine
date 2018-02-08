// Demo.cpp : Defines the entry point for the console application.
//
#include <tchar.h>
#include <Injector/Injector.h>
#include <chrono>
#include <IInput_API.h>
#include <iostream>
#include <thread>
#include <IPCore\Core\ICore.h>
#include <ISimpleSceneModule_API.h>
#include <ISimpleContentModule_API.h>
#include <ApplicationBase/Application.h>
#include <IPhysicsModule_API.h>
boost::shared_ptr<IInput_API> input;

#define DEP_GRAPH_PATH "../Assets/Dependencygraph/TestDepGraph.xml"
#ifdef _DEBUG
#define MODULE_PATH "../Output/Win32/Debug"
#else
#define MODULE_PATH "../Output/Win32/Release"
#endif


//void WriteInputs(TaskContext& c)
//{
//	system("cls");
//	Scheduler::SchedInfo& d = c;
//	std::cout << std::this_thread::get_id();
//	std::cout << d.dt << std::endl;
//	auto memes = input->getInputBuffered(d.dt / 1000000, true);
//	std::time_t end_time;
//	for (auto m : memes)
//	{
//		end_time = std::chrono::system_clock::to_time_t(m.timeStamp);
//		if (m.type == IInput::InputType::INPUT_DEVICE_DISCONNECTED)
//			std::cout << "Type: " << (int)m.type << " | TimeStamp: " << std::ctime(&end_time) << " | Data: " << m.data.kd.keycode << std::endl;// " | Data: " << m.data.i3dmd.yaw << " - " << m.data.i3dmd.pitch << " - " << m.data.i3dmd.roll << std::endl;
//	}
//}

class MyApp : public ipengine::Application
{
public:
	// Geerbt über Application
	virtual void initialize() override
	{
		auto mods = getInjector().getLoadedModules();
		auto ssm = boost::dynamic_pointer_cast<ISimpleSceneModule_API>(mods["SimpleSceneModule"]);
		auto scm = boost::dynamic_pointer_cast<SCM::ISimpleContentModule_API>(mods["SimpleContentModule"]);
		auto phys = boost::dynamic_pointer_cast<IPhysicsModule_API>(mods["PhysicsModule"]);
		auto sceneid = ssm->LoadSceneFromFile("../Assets/Scenes/TestScene.xml");
		ssm->SwitchActiveScene(sceneid);
		phys->createcloth();
		auto entity = scm->getEntityById(0);
		std::cout << sceneid << "\n";
		std::cout << SCM::allEntitiesAsString(*scm, true);
		std::cout << SCM::allMeshObjectsAsString(*scm, true);
	}

	virtual void onShutdown() override
	{
		//do shutdown stuff
	}

	virtual void preTick() override
	{

	}

	virtual void postTick(const ipengine::Time& t) override
	{

	}

	virtual void onConsole() override
	{
		//Do console stuff here
	}

	void runapp()
	{
		run();
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	MyApp app;
	app.init("../Assets/Config/config.cfg");
	app.run();

//#ifdef _DEBUG
//	Injector inj("../Assets/Dependencygraph/TestDepGraph.xml", "../Output/Debug");
//#else
//	Injector inj("../Assets/Dependencygraph/TestDepGraph.xml", "../Output/Release");
//#endif
//	ipengine::Core core;
//	inj.LoadModules(&core);
//
//	auto mods = inj.getLoadedModules();
//	auto ssm = boost::dynamic_pointer_cast<ISimpleSceneModule_API>(mods["SimpleSceneModule"]);
//	auto scm = boost::dynamic_pointer_cast<SCM::ISimpleContentModule_API>(mods["SimpleContentModule"]);
//	auto sceneid = ssm->LoadSceneFromFile("../Assets/Scenes/TestScene.xml");
//	ssm->SwitchActiveScene(sceneid);
//	auto entity = scm->getEntityById(0);
//	std::cout << sceneid << "\n";
//	std::cout << SCM::allEntitiesAsString(*scm, true);
//	std::cout << SCM::allMeshObjectsAsString(*scm, true);
//	//core.getThreadPool().startWorkers();
//	//auto b = core.getThreadPool().isRunning();
//	//boost::shared_ptr<IScheduler_API> schedAPI = boost::dynamic_pointer_cast<IScheduler_API>(mods["Scheduler"]);
//	//IInput_API input = dynamic_cast<IInput_API>(mods["InputModule"]);
//
//	//[input]()->void {
//	//	std::chrono::system_clock clock;
//	//	std::chrono::system_clock::duration delta;
//	//	std::chrono::system_clock::time_point lastTick;
//	//	try {
//	//		delta = clock.now() - lastTick;
//	//		if (delta > std::chrono::milliseconds(200))
//	//		{
//	//			system("cls");
//	//			std::cout << delta.count() << std::endl;
//	//			auto memes = input->getInputBuffered(delta.count() / 10000, true);
//	//			std::time_t end_time;
//	//			for (auto m : memes)
//	//			{
//	//				end_time = std::chrono::system_clock::to_time_t(m.timeStamp);
//	//				if (m.type == IInput::InputType::INPUT_DEVICE_DISCONNECTED)
//	//					std::cout << "Type: " << (int)m.type << " | TimeStamp: " << std::ctime(&end_time) << " | Data: " << m.data.kd.keycode << std::endl;// " | Data: " << m.data.i3dmd.yaw << " - " << m.data.i3dmd.pitch << " - " << m.data.i3dmd.roll << std::endl;
//	//			}
//	//			lastTick = clock.now();
//	//		}
//	//	}
//	//	catch (std::exception e)
//	//	{
//	//		auto meme = "t";
//	//	}};
//	while (true) {
//		core.getScheduler().schedule();
//	};
//	
//		
//	//std::string meme;
//	//std::cin >> meme;
	return 0;
}

