// CoreTetst.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Core/ICore.h"
#include "Injector.h"
#include "IModulCoreTest_API.h"
int main()
{
	Injector inj("dep.xml", "../Debug");
	inj.LoadModules();
	ipengine::Core* mycore = new ipengine::Core();
	auto m = inj.getLoadedModules();
	auto coretest = boost::dynamic_pointer_cast<IModulCoreTest_API>(m["ModulCoreTest"]);
	coretest->giveSched(mycore);
	coretest->start();
	auto t = std::chrono::high_resolution_clock::now();
	while (true) {
		//std::this_thread::sleep_for(std::chrono::milliseconds(500));
		//std::cout << "Schedule mit Delta: " << std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - t).count() << std::endl;
		t = std::chrono::high_resolution_clock::now();
		mycore->getScheduler().schedule();
		//std::cout << "Schedule nach mit Delta: " << std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - t).count() << std::endl;
		t = std::chrono::high_resolution_clock::now();
	};
    return 0;
}

