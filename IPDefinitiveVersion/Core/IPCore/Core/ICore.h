#ifndef _I_CORE_H_
#define _I_CORE_H_
#include <IPCore/core_config.h>
#include <IPCore/ThreadingServices/TaskLib.h>
#include <IPCore/RuntimeControl/Scheduler.h>
#include <IPCore/RuntimeControl/Console.h>
#include <IPCore/Messaging/EndpointRegistry.h>
#include <IPCore/Config/ConfigManager.h>
#include <IPCore/Memory/MemoryManager.h>

#include <IPCore/Core/ICoreTypes.h>
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
		Time tick(bool& shouldstop); //TODO: figure out how to shutdown the engine

		//id stuff
		ipid createID();
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

		//Platform Services //internally interfaces with PAL 

	private:
		void setupCoreConsoleCommands();

		//command handlers
		void cmd_shutdown(const ConsoleParams& params);
		void cmd_listcommands(const ConsoleParams& params);

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

		
		//Core messaging endpoint
		MessageEndpoint* core_msgep;

		//single atomic for generating global ids. 0 is always an invalid id.
		std::atomic<ipid> core_idgen;

		//flag to indicate stop
		std::atomic<bool> m_isstopping;
		std::atomic<bool> m_stopped;
	};
}


#endif // !_I_CORE_H_

