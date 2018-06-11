#ifndef _ERROR_MANAGER_H_
#define _ERROR_MANAGER_H_
#include <core/core_config.h>
#include <core/core_types.h>
#include <core/util/function.h>
#include <atomic>
#include <core/util/spinlock.h>
#include <mutex>
#include <core/debug_monitoring/Error.h>


namespace ipengine
{
	using ErrorHandlerFunc = ipengine::function<void(ipex&)>;
	class ErrorHandler;
	class CORE_API ErrorManager
	{
	public:
		ErrorManager();
		~ErrorManager();


		void reportException(ipex& ex);
		void registerHandlerFunc(const ErrorHandlerFunc& ehf);
		void handlePendingExceptions();
		ErrorHandler createHandlerInstance();

	private:
		YieldingSpinLock<4000> m_eq_mtx;
		std::vector<ipex> pendingErrors;
		std::atomic<bool> errorsPending;
		ErrorHandlerFunc ehandler;
	};
}
#endif