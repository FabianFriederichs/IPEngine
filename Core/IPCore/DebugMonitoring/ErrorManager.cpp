#include "ErrorManager.h"
#include "ErrorHandler.h"

ipengine::ErrorManager::ErrorManager() :
	errorsPending(false)
{

}

ipengine::ErrorManager::~ErrorManager()
{

}

void ipengine::ErrorManager::reportException(ipex & ex)
{
	std::lock_guard<YieldingSpinLock<4000>> lock(m_eq_mtx);
	pendingErrors.push_back(ex);
	errorsPending.store(true, std::memory_order_relaxed);
}

void ipengine::ErrorManager::registerHandlerFunc(const ErrorHandlerFunc & ehf)
{
	std::lock_guard<YieldingSpinLock<4000>> lock(m_eq_mtx);
	ehandler = ehf;
}

void ipengine::ErrorManager::handlePendingExceptions()
{
	if (errorsPending.load(std::memory_order_relaxed))
	{
		std::lock_guard<YieldingSpinLock<4000>> lock(m_eq_mtx);
		for (auto& ex : pendingErrors)
		{
			if (!ehandler.isEmpty())
				ehandler(ex);
		}
		pendingErrors.clear();
		errorsPending.store(false, std::memory_order_relaxed);
	}
}

ipengine::ErrorHandler ipengine::ErrorManager::createHandlerInstance()
{
	return ErrorHandler(this);
}
