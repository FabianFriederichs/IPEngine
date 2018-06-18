/** \addtogroup debug
Implements debug and error handling functionality of the core.
*  @{
*/

/*!
\file ErrorManager.h
*/

#ifndef _ERROR_MANAGER_H_
#define _ERROR_MANAGER_H_
#include <IPCore/core_config.h>
#include <IPCore/Core/ICoreTypes.h>
#include <IPCore/Util/function.h>
#include <atomic>
#include <IPCore/Util/spinlock.h>
#include <mutex>
#include <IPCore/DebugMonitoring/Error.h>


namespace ipengine
{
	/*!
	\brief Callback type for error handler callbacks

	This is an instantiation of the ipengine::function template.
	Error handler callbacks must have this signature.
	*/
	using ErrorHandlerFunc = ipengine::function<void(ipex&)>;
	class ErrorHandler;

	/*!
	\brief Implements error reporting to the core.

	The ErrorManager queues up exceptions handed over from the modules via the ErrorHandler instances.
	The core uses the handlePendingExceptions function to print and log them, as well as shutting the
	system down if fatal exceptions occur.

	Clients of the core should query their ErrorHandler instances by calling the createHandlerInstance function.
	*/
	class CORE_API ErrorManager
	{
	public:
		ErrorManager();
		~ErrorManager();

		/*!
		\brief Queues up exceptions.

		This function is mostly called by the ErrorHandler instances and should not be called
		directly by the modules.
		\param[in] ex	Reference to an exception object to enqueue.
		*/
		void reportException(ipex& ex);

		/*!
		\brief The core uses this function to register a callback that handles all incoming exceptions.
		\param[in] ehf		The callback to be registered
		*/
		void registerHandlerFunc(const ErrorHandlerFunc& ehf);

		/*!
		\brief The core calls this function in every tick cycle.

		Enqueued exceptions are passed to the registered handler one by one.
		*/
		void handlePendingExceptions();

		/*!
		\brief Create an ErrorHandler instance which reports all exceptions to this ErrorManager instance.
		\returns Returns an ErrorHandler instance that can be used by the modules to handle exceptions.
		*/
		ErrorHandler createHandlerInstance();

	private:
		YieldingSpinLock<4000> m_eq_mtx; //!< To ensure mutual exclusion on the pendingErrors member.
		std::vector<ipex> pendingErrors; //!< Stores pending exceptions.
		std::atomic<bool> errorsPending; //!< Indicates that there are exceptions to be handled.
		ErrorHandlerFunc ehandler; //!< Handler callback object used to handle pending exceptions.
	};
}
#endif

/** @}*/