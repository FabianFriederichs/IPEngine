#include "IModule_API.h"
#include "Scheduler.h"

class  IScheduler_API : public IModule_API
{
public:
	virtual void schedule() = 0;
	virtual int subscribe(const function<void(TaskContext&)>& schedfunc, Scheduler::interval_t desiredInterval, Scheduler::SubType type, float timescale) = 0;
};