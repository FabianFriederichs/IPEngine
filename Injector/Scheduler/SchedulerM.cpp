// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "SchedulerM.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
SchedulerM::SchedulerM()
{
	m_info.identifier = "Scheduler";
	m_info.version = "1.0";
	m_info.iam = "IScheduler_API";
	return;
}

void SchedulerM::schedule()
{
	m_sched.schedule();
}

int SchedulerM::subscribe(const function<void(TaskContext&)>& schedfunc, Scheduler::interval_t desiredInterval, Scheduler::SubType type, float timescale)
{
	auto v = m_sched.subscribe(schedfunc, desiredInterval, type, timescale);
	handles.push_back(v);
	return v.getID();
}
