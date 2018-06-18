/** \addtogroup core
Combines all the core components into a single interface.
*  @{
*/

/*!
\file ICore.h
\brief Defines the core interface
*/

#ifndef _I_CORE_H_
#define _I_CORE_H_
#include <IPCore/core_config.h>
#include <IPCore/ThreadingServices/TaskLib.h>
#include <IPCore/RuntimeControl/Scheduler.h>
#include <IPCore/RuntimeControl/Console.h>
#include <IPCore/Messaging/EndpointRegistry.h>
#include <IPCore/Config/ConfigManager.h>
#include <IPCore/Memory/MemoryManager.h>
#include <IPCore/DebugMonitoring/ErrorManager.h>
#include <IPCore/Util/idgen.h>

#include <IPCore/Core/ICoreTypes.h>
//TODO: create core interface and core implementation. Maybe pimpl the whole thing
namespace ipengine
{
	/*!
	\brief Combines all the core functionality and manages neccessary instances of core subsystems
	
	The Core class holds instances of several classes which implement certain parts of the core functionality.
	There are getters providing access to those instances.

	Beside that, general runtime logic is implemented by the Core class itself:
	\li Initialization
	\li Tick logic (see the tick member function for details)
	\li Error management
	\li Generation of unique IDs

	When building the core, many warnings are reported by the compiler. This is because much of the implementation detail is not hidden yet.
	To eliminate this problem some refactoring must be done in the future.
	*/
	class CORE_API Core
	{
	public:
		//! Creates an uninitialized core instance.
		Core();
		//! Destructor
		~Core();

		/*!
		\brief Initializes the core instance.

		Initializes the core instance. A valid path to a config file is passed with at least the following content:
		
		\code{.unparsed}
		core
			threading
				# define how many worker threads shall be created
				max_worker_threads = 4
			resources		
				asset_path = "path/to/some/asset/directory"
		\endcode

		\param[in] configPath	The path to the config file
		*/
		void initialize(const iprstr configPath);
		/*!
		\brief Tells the thread pool to start the workers.
		*/
		void run();
		/*!
		\brief Does some cleanup.
		*/
		void shutdown();
		/*!
		\brief Stops all workers.

		Stops all workers and tells the application that drives the core (via the tick() member function) to stop.
		*/
		void stop();
		/*!
		\brief The 'clock' input of the core.

		The tick function is used to drive the core. A basic implementation looks like the following:

		\code{.cpp}
		bool shouldstop = false;
		while(!shouldstop)
		{
			core.tick(shouldstop);
		}
		\endcode

		\param[out]		Is set to true when the core is about to stop. If the parameter was set to true, the caller should stop
						calling tick(), as shown in the example above.
		\returns		The time that has passed during the core tick.
		*/
		Time tick(bool& shouldstop);

		/*!
		\brief Invokes the core's error handling logic for a given ipex exception.
		\param[in] Reference to the ipex object that should be handled.
		*/
		void handleError(ipex& ex);

		/*!
		\brief Returns a unique ID

		Returns a unique ID that is unique for the entire runtime of the application.

		\returns Returns a unique ID.
		*/
		ipid createID();
		/*!
		\brief returns a reference to the IdGen instance of the core.

		Returns a reference to the id generator instance of the core.

		\returns Reference to an IdGen instance.
		*/
		IdGen& getIDGen();
		/*!
		\brief Returns a reference to the Console instance.
		\returns Reference to Console instance.
		*/ 
		Console& getConsole();
		/*!
		\brief Returns a reference to the Scheduler instance.
		\returns Reference to Scheduler instance.
		*/
		Scheduler& getScheduler();		
		/*!
		\brief Returns a reference to the ThreadPool instance.
		\returns Reference to ThreadPool instance.
		*/
		ThreadPool& getThreadPool();
		/*!
		\brief Returns a reference to the EndpointRegistry instance.
		\returns Reference to EndpointRegistry instance.
		*/
		EndpointRegistry& getEndpointRegistry();
		/*!
		\brief Returns a reference to the MemoryManager instance.
		\returns Reference to MemoryManager instance.
		*/
		MemoryManager& getMemoryManager();
		/*!
		\brief Returns a reference to the ConfigManager instance.
		\returns Reference to ConfigManager instance.
		*/
		ConfigManager& getConfigManager();
		/*!
		\brief Returns a reference to the ErrorManager instance.
		\returns Reference to ErrorManager instance.
		*/
		ErrorManager& getErrorManager();

	private:
		//! Registers some core commands with the Console instance.
		void setupCoreConsoleCommands();

		//command handlers
		//! Shutdown command handler
		void cmd_shutdown(const ConsoleParams& params);
		//! Help command handler
		void cmd_listcommands(const ConsoleParams& params);
		//! Detailed help cómmand handler
		void cmd_listcommandsd(const ConsoleParams& params);

		//Subsystem implementing class-objects
		//Question: abtract interface for each subsystem? (This way we could simply publish those interfaces and reduce the uglyness of this header.
		//TODO: pimpl the core!
		//idea: pack content of "core modules" and combine them with an interface. only store pointers to these interfaces.
		
		//! Pointer to the ThreadPool instance
		ThreadPool* cmodule_threadingservices;
		//! Pointer to the Scheduler instance
		Scheduler* cmodule_scheduler;
		//! Pointer to the Console instance
		Console* cmodule_console;
		//! Pointer to the EndpointRegistry instance
		EndpointRegistry* cmodule_endpointregistry;
		//! Pointer to the ConfigManager instance
		ConfigManager* cmodule_configmanager;
		//! Pointer to the MemoryManager instance
		MemoryManager* cmodule_memorymanager;
		//! Pointer to the ErrorManager instance
		ErrorManager* cmodule_errormanager;

		
		//Core messaging endpoint
		//! Mesaging endpoint of the core
		EndpointHandle core_msgep;

		//single atomic for generating global ids. 0 is always an invalid id.
		//! ID-generator instance
		IdGen core_idgen;

		//flag to indicate stop
		//! Atomic bool which holds the "running"-state of the core.
		std::atomic<bool> m_isrunning;
	};
}


#endif // !_I_CORE_H_

/** @}*/