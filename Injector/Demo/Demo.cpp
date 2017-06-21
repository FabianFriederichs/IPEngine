// Demo.cpp : Defines the entry point for the console application.
//
#include <tchar.h>
#include "Injector.h"
#include <chrono>
#include <IInput_API.h>
#include <iostream>
#include <thread>



int _tmain(int argc, _TCHAR* argv[])
{
	Injector inj("XMLFile.xml", "../Debug");
	inj.LoadModules();
	auto mods = inj.getLoadedModules();
	boost::shared_ptr<IInput_API> input = boost::dynamic_pointer_cast<IInput_API>(mods["InputModule"]);
	//IInput_API input = dynamic_cast<IInput_API>(mods["InputModule"]);
	
	std::thread meme([input]()->void{
		std::chrono::high_resolution_clock clock;
		std::chrono::high_resolution_clock::duration delta;
		std::chrono::high_resolution_clock::time_point lastTick;
		while (1)
		{
			try{
			delta = clock.now() - lastTick;
			if (delta > std::chrono::milliseconds(500))
			{
				std::cout << delta.count() << std::endl;
				auto memes = input->getInputBuffered(delta.count()/10000);
				std::time_t end_time;
				for (auto m : memes)
				{
					end_time = std::chrono::high_resolution_clock::to_time_t(m.timeStamp);
					std::cout << "Type: " << (int)m.type << " | TimeStamp: " << std::ctime(&end_time) << " | Data: " << m.data.md.x << " - " << m.data.md.y;
				}
				lastTick = clock.now();
			}
			}
			catch (std::exception e)
			{
				auto meme = "t";
			}
			};
});
	while (1);
	//std::string meme;
	//std::cin >> meme;
	return 0;
}

