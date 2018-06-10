#include "ErrorHandler.h"



ipengine::ErrorHandler::ErrorHandler() :
	emanager(nullptr),
	last(),
	haserr(false),
	ehfunc()
{}

ipengine::ErrorHandler::ErrorHandler(ErrorManager * manager) :
	emanager(manager),
	last(),
	haserr(false),
	ehfunc()
{}

ipengine::ErrorHandler::ErrorHandler(const ErrorHandler & other) :
	emanager(other.emanager),
	last(other.last),
	haserr(other.haserr.load()),
	ehfunc(other.ehfunc)
{}

ipengine::ErrorHandler::~ErrorHandler()
{}

ipengine::ErrorHandler & ipengine::ErrorHandler::operator=(const ErrorHandler & other)
{
	if (this == &other)
		return *this;
	emanager = other.emanager;
	last = other.last;
	haserr.store(other.haserr.load());
	ehfunc = other.ehfunc;
	return *this;
}

void ipengine::ErrorHandler::registerCustomHandler(const ErrorHandlerFunc & ehfunc)
{
	std::lock_guard<YieldingSpinLock<4000>> lock(last_ex_mtx);
	this->ehfunc = ehfunc;
}

void ipengine::ErrorHandler::handleException(ipex & ex)
{
	std::lock_guard<YieldingSpinLock<4000>> lock(last_ex_mtx);
	last = ex;
	if(!ehfunc.isEmpty())
		ehfunc(last);
	if(emanager)
		emanager->reportException(last);
	haserr.store(true, std::memory_order_relaxed);
}

ipengine::ipex ipengine::ErrorHandler::getLastError()
{
	std::lock_guard<YieldingSpinLock<4000>> lock(last_ex_mtx);
	return last;
}

bool ipengine::ErrorHandler::hasError()
{
	return haserr.load(std::memory_order_relaxed);
}

void ipengine::ErrorHandler::clear()
{
	haserr.store(false, std::memory_order_relaxed);
}