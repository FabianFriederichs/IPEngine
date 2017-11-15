// Demo.cpp : Defines the entry point for the console application.
//
#include <tchar.h>
#include <Injector/Injector.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <IPCore\Core\ICore.h>
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

int _tmain(int argc, _TCHAR* argv[])
{
#ifdef _DEBUG
	Injector inj("../Assets/Dependencygraph/TestDepGraph.xml", "../Output/Debug");
#else
	Injector inj("../Assets/Dependencygraph/TestDepGraph.xml", "../Output/Release");
#endif
	ipengine::Core core;
	inj.LoadModules(&core);
	core.getThreadPool().startWorkers();

	while (true) {
		core.getScheduler().schedule();
	};
	
	return 0;
}

