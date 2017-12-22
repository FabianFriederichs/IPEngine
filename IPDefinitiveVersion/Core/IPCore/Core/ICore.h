#ifndef _I_CORE_H_
#define _I_CORE_H_
#include <IPCore/core_config.h>
#include <IPCore/ThreadingServices/TaskLib.h>
#include <IPCore/RuntimeControl/Scheduler.h>
#include <IPCore/RuntimeControl/Console.h>
#include <IPCore/Messaging/EndpointRegistry.h>

namespace ipengine
{
	//this interface contains the core functionality
	class CORE_API Core
	{
	public:
		//ctors
		Core();
		~Core();

		void initialize();
		void shutdown();
		Time tick(bool& shouldstop);

		//id stuff
		ipid createID();
		//Console  
		Console& getConsole();
		//Runtime Control 
		Scheduler& getScheduler();		
		//Threading Services
		ThreadPool& getThreadPool();
		//Messaging Services -
		EndpointRegistry& getEndpointRegistry();

		//Memory Manager  

		//Configuration Manager -                
		
		//Debug/Monitoring 

		//Platform Services //internally interfaces with PAL 

	private:
		//Subsystem implementing class-objects
		//Question: abtract interface for each subsystem? (This way we could simply publish those interfaces and reduce the uglyness of this header.
		//TODO: pimpl the core!
		ThreadPool cmodule_threadingservices;
		Scheduler cmodule_scheduler;
		Console cmodule_console;
		EndpointRegistry cmodule_endpointregistry;
		
		
		MessageEndpoint* core_msgep;

		//single atomic for generating global ids. 0 is always an invalid id.
		std::atomic<ipid> core_idgen;
	};
}


#endif // !_I_CORE_H_

