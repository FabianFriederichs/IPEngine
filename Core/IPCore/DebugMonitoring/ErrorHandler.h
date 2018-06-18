/** \addtogroup debug
*  @{
*/

/*!
\file ErrorHandler.h
\brief Defines the error handler class
*/

#ifndef _ERROR_HANDLER_H_
#define _ERROR_HANDLER_H_
#include <IPCore/core_config.h>
#include <IPCore/Core/ICoreTypes.h>
#include <IPCore/Util/function.h>
#include <atomic>
#include <IPCore/Util/spinlock.h>
#include <mutex>
#include <IPCore/DebugMonitoring/Error.h>
#include <IPCore/DebugMonitoring/ErrorManager.h>

//! Begin an exception-handled code block
#define BEGINEX try {

/*!
\brief End an exception-handled code block

Ends an exception-handled code block.
\param[in] handler	The ErrorHandler instance that should be used to handle all occuring exceptions.
*/
#define ENDEX(handler) }											\
catch(ipengine::ipex& ex)											\
{																	\
	handler.handleException(ex);									\
}																	\
catch(std::exception& ex)											\
{																	\
	handler.handleException(ipengine::ipex(ex.what()));				\
}																	\
catch(...)															\
{																	\
	handler.handleException(ipengine::ipex("unknown exception"));	\
}																	\

namespace ipengine
{
	/*!
	\brief Implements a helper class that is used to handle exceptions at module interface boundaries.

	Exceptions must not be thrown accross shared-library boundaries. That means that all exceptions
	have to be catched just beneath the interface layer. This class makes catching unhandled exceptions
	convenient.

	To create an instance of ErrorHandler, the respective ErrorManager instance in the core should be
	asked to do that. After that, a custom callback can be registered to implement module-specific
	exception logic. All occured exceptions are reported to the ErrorManager instance for
	printing and logging purposes.

	The two macros BEGINEX and ENDEX are defined to remove the need of putting try-catch blocks everywhere
	by hand. Exception handling of a block of code is done like the following:

	\code{.cpp}
	BEGINEX

	//arbitrary code
	//...
	//...

	ENDEX(errorHandlerInstance)
	\endcode

	Any exceptions that are not of type ipex are converted to an ipex object with fatal severity.
	*/
	class CORE_API ErrorHandler
	{
	private:
		ErrorManager* emanager; //!< Pointer to ErrorManager instance
		ipex last; //!< The last occured exception
		ErrorHandlerFunc ehfunc; //!< Callback for custom exception handling
		std::atomic<bool> haserr; //!< Indicates that the entity that owns this handler instance is in an errorneous state
		YieldingSpinLock<4000> last_ex_mtx; //!< Mutex for preventing data races on the last member

	public:
		ErrorHandler(); //!< Default constructor. Initializes the handler with no ErrorManager attached => Catched exceptions are not reported to the core.
		ErrorHandler(ErrorManager* manager); //!< Initializes the handler with an ErrorManager pointer. => Catched exceptions are reported to the core.
		ErrorHandler(const ErrorHandler& other); //!< Copy constructor
		~ErrorHandler(); //!< Desctructor

		ErrorHandler& operator=(const ErrorHandler& other); //!< Copy assignment operator

		/*!
		\brief Register a custom handler callback

		If a valid callback is registered, it is called for every exception that occurs.
		\param[in] ehfunc	The callback to be registered
		*/
		void registerCustomHandler(const ErrorHandlerFunc& ehfunc);

		/*!
		\brief Manually pass an ipex oject to be handled
		\param[in] ex	A reference to the ipex object that should be handled by the ErrorHandler
		*/
		void handleException(ipex& ex);

		/*!
		\brief Returns a copy of the last exception that occured.
		\returns		A copy of the last-handled ipex object
		*/
		ipex getLastError();

		/*!
		\brief Returns true if there was an exception.

		Can be used to signal errorneus state of the owning entity. If an exception was handled, hasError()
		returns true until the owning entity calls clear() to signal that it has recovered from an errorneous state.

		\returns	true, if an exception was handled an clear was not called yet.
		*/
		bool hasError();

		/*!
		\brief Signals recovering from an errorneous state.

		This member function should be called from the owning entity to signal that it has recovered from an
		errorneous state after an exception occured.
		*/
		void clear();
	};
}
#endif
/** @}*/