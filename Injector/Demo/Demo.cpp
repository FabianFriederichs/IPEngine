// Demo.cpp : Defines the entry point for the console application.
//
#include <tchar.h>
#include "Injector.h"
#include <chrono>
#include <IInput_API.h>
#include <iostream>
#include <thread>
#include "IScheduler_API.h"

boost::shared_ptr<IInput_API> input;
void WriteInputs(TaskContext& c)
{
	system("cls");
	Scheduler::SchedInfo& d = c;
	std::cout << std::this_thread::get_id();
	std::cout << d.dt << std::endl;
	auto memes = input->getInputBuffered(d.dt / 1000000, true);
	std::time_t end_time;
	for (auto m : memes)
	{
		end_time = std::chrono::system_clock::to_time_t(m.timeStamp);
		if (m.type == IInput::InputType::INPUT_DEVICE_DISCONNECTED)
			std::cout << "Type: " << (int)m.type << " | TimeStamp: " << std::ctime(&end_time) << " | Data: " << m.data.kd.keycode << std::endl;// " | Data: " << m.data.i3dmd.yaw << " - " << m.data.i3dmd.pitch << " - " << m.data.i3dmd.roll << std::endl;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	Injector inj("XMLFile.xml", "../Release");
	inj.LoadModules();
	auto mods = inj.getLoadedModules();
	input = boost::dynamic_pointer_cast<IInput_API>(mods["InputModule"]);
	boost::shared_ptr<IScheduler_API> schedAPI = boost::dynamic_pointer_cast<IScheduler_API>(mods["Scheduler"]);
	//IInput_API input = dynamic_cast<IInput_API>(mods["InputModule"]);

	//[input]()->void {
	//	std::chrono::system_clock clock;
	//	std::chrono::system_clock::duration delta;
	//	std::chrono::system_clock::time_point lastTick;
	//	try {
	//		delta = clock.now() - lastTick;
	//		if (delta > std::chrono::milliseconds(200))
	//		{
	//			system("cls");
	//			std::cout << delta.count() << std::endl;
	//			auto memes = input->getInputBuffered(delta.count() / 10000, true);
	//			std::time_t end_time;
	//			for (auto m : memes)
	//			{
	//				end_time = std::chrono::system_clock::to_time_t(m.timeStamp);
	//				if (m.type == IInput::InputType::INPUT_DEVICE_DISCONNECTED)
	//					std::cout << "Type: " << (int)m.type << " | TimeStamp: " << std::ctime(&end_time) << " | Data: " << m.data.kd.keycode << std::endl;// " | Data: " << m.data.i3dmd.yaw << " - " << m.data.i3dmd.pitch << " - " << m.data.i3dmd.roll << std::endl;
	//			}
	//			lastTick = clock.now();
	//		}
	//	}
	//	catch (std::exception e)
	//	{
	//		auto meme = "t";
	//	}};
	schedAPI->subscribe(TaskFunction::make_func(WriteInputs), (int)((500)*1000000.f), Scheduler::SubType::Interval, 1);
	while (true) {
		schedAPI->schedule();
	}
	
	
		
	//std::string meme;
	//std::cin >> meme;
	return 0;
}

