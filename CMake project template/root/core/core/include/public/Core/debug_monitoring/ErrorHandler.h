#ifndef _ERROR_HANDLER_H_
#define _ERROR_HANDLER_H_
#include <core/core_config.h>
#include <core/core_types.h>
#include <core/util/function.h>
#include <atomic>
#include <core/util/spinlock.h>
#include <mutex>
#include <core/debug_monitoring/Error.h>
#include <core/debug_monitoring/ErrorManager.h>

//convenience macro for standard try catch
#define BEGINEX try {

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
	class CORE_API ErrorHandler
	{
	private:
		ErrorManager* emanager;
		ipex last;
		ErrorHandlerFunc ehfunc;
		std::atomic<bool> haserr;
		YieldingSpinLock<4000> last_ex_mtx;

	public:
		ErrorHandler();
		ErrorHandler(ErrorManager* manager);
		ErrorHandler(const ErrorHandler& other);
		~ErrorHandler();

		ErrorHandler& operator=(const ErrorHandler& other);

		void registerCustomHandler(const ErrorHandlerFunc& ehfunc);
		void handleException(ipex& ex);

		ipex getLastError();
		bool hasError();
		void clear();
	};
}
#endif