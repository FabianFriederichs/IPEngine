#ifndef _I_CORE_H_
#define _I_CORE_H_
#include "core_config.h"
#include "ThreadingServices/TaskLib.h"
#include "RuntimeControl/Scheduler.h"
#include "RuntimeControl/Console.h"

namespace ipengine
{
	//this interface contains the core functionality
	class CORE_API Core
	{
	public:
		//ctors
		Core();
		~Core();

		//core
		void initialize();
		void shutdown();




		//Timing -

		//Console  
		Console& getConsole();
		//Runtime Control 
		Scheduler& getScheduler();		
		//Threading Services
		ThreadPool& getThreadPool();

		//Data Store/Repository -
		//?
		//Messaging Services -

		//Memory Manager  

		//Configuration Manager -                
		
		//Debug/Monitoring 

		//Platform Services //internally interfaces with PAL 

	private:
		//Subsystem implementing class-objects
		//Question: abtract interface for each subsystem? (This way we could simply publish those interfaces and reduce the uglyness of this header.
		ThreadPool cmodule_threadingservices;
		Scheduler cmodule_scheduler;
		Console cmodule_console;

	};
}


#endif // !_I_CORE_H_

