#ifndef _I_CORE_H_
#define _I_CORE_H_
#include <core/core_config.h>
#include <core/threading/TaskLib.h>
#include <core/runtime/Scheduler.h>
#include <core/runtime/Console.h>
#include <core/messaging/EndpointRegistry.h>
#include <core/config/ConfigManager.h>
#include <core/memory/MemoryManager.h>
#include <core/debug_monitoring/ErrorManager.h>
#include <core/util/idgen.h>

#include <core/core_types.h>
//TODO: create core interface and core implementation. Maybe pimpl the whole thing
namespace ipengine
{
	//this interface contains the core functionality (what the fuck that sounds wrong)
	class CORE_API Core
	{
	public:
		//ctors
		Core();
		~Core();

		void initialize(const iprstr configPath);
		void run();
		void shutdown();
		void stop();
		Time tick(bool& shouldstop); //TODO: figure out how to shutdown the engine
		void handleError(ipex& ex);

		//id stuff
		ipid createID();
		IdGen& getIDGen();
		//Console  
		Console& getConsole();
		//Runtime Control 
		Scheduler& getScheduler();		
		//Threading Services
		ThreadPool& getThreadPool();
		//Messaging Services
		EndpointRegistry& getEndpointRegistry();
		//Memory Manager  
		MemoryManager& getMemoryManager();
		//Configuration Manager                
		ConfigManager& getConfigManager();
		//Debug/Monitoring 
		ErrorManager& getErrorManager();
		//Platform Services //internally interfaces with PAL

	private:
		void setupCoreConsoleCommands();

		//command handlers
		void cmd_shutdown(const ConsoleParams& params);
		void cmd_listcommands(const ConsoleParams& params);
		void cmd_listcommandsd(const ConsoleParams& params);

		//Subsystem implementing class-objects
		//Question: abtract interface for each subsystem? (This way we could simply publish those interfaces and reduce the uglyness of this header.
		//TODO: pimpl the core!
		//idea: pack content of "core modules" and combine them with an interface. only store pointers to these interfaces.
		ThreadPool* cmodule_threadingservices;
		Scheduler* cmodule_scheduler;
		Console* cmodule_console;
		EndpointRegistry* cmodule_endpointregistry;
		ConfigManager* cmodule_configmanager;
		MemoryManager* cmodule_memorymanager;
		ErrorManager* cmodule_errormanager;

		
		//Core messaging endpoint
		EndpointHandle core_msgep;

		//single atomic for generating global ids. 0 is always an invalid id.
		IdGen core_idgen;

		//flag to indicate stop
		std::atomic<bool> m_isrunning;
	};
}


#endif // !_I_CORE_H_

