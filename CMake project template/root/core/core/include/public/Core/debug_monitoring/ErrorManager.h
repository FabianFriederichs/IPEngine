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