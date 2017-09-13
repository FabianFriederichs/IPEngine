#include "Scheduler.h"

// class scheduler -----------------------------------------------------------

// private methods
Scheduler::Scheduler() :
	m_idgen(0),
	m_subscriptions()
{
}


Scheduler::~Scheduler()
{
	for (auto s : m_subscriptions)
	{
		unsubscribe(s.second);
	}
}


bool Scheduler::unsubscribe(SchedSub * s)
{
	std::unique_lock<YieldingSpinLock<5000>> lock(m_sublock);
	bool ret = m_subscriptions.erase(s->subid);
	lock.unlock();
	delete s;
	return ret;
}

void Scheduler::update(const SubChange & change)
{
	m_changequeue.enqueue(change);
}

void Scheduler::applyChanges()
{
	SubChange sc;
	while (m_changequeue.try_dequeue(sc))
	{
		std::lock_guard<YieldingSpinLock<5000>> lock(m_sublock);
		SchedSub* sub = m_subscriptions[sc.subid];
		sub->interval = sc.interval;
		sub->timescale = sc.timescale;
		sub->type = sc.type;
	}
}

void Scheduler::schedule()
{
}

//public member functions


Scheduler::SubHandle Scheduler::subscribe(const function<void(TaskContext&)>& schedfunc, interval_t desiredInterval, SubType type, float timescale, ThreadPool* pool)
{	
	SchedSub* sub = new SchedSub(m_idgen.fetch_add(1, std::memory_order_relaxed),
		pool->createTask(schedfunc, TaskContext(SchedInfo())),
		type,
		desiredInterval,
		timescale);
	std::unique_lock<YieldingSpinLock<5000>> lock(m_sublock);
	m_subscriptions.insert(sub_entry_t(sub->subid, sub));
	lock.unlock();
	return SubHandle(sub, this);
}


// class subhandle ----------------------------------------------------------

Scheduler::SubHandle::SubHandle(SchedSub * sub, Scheduler * sched) :
	m_subscription(sub),
	m_sched(sched)
{
	sub->refct.inc();
}

Scheduler::SubHandle::SubHandle() :
	m_subscription(nullptr),
	m_sched(nullptr)
{
}

Scheduler::SubHandle::SubHandle(const SubHandle & other) :
	m_subscription(other.m_subscription),
	m_sched(other.m_sched)
{
	if(m_subscription != nullptr)
		m_subscription->refct.inc();
}

Scheduler::SubHandle::SubHandle(SubHandle && other) :
	m_subscription(other.m_subscription),
	m_sched(other.m_sched)
{
	other.m_sched = nullptr;
	other.m_subscription = nullptr;
}

Scheduler::SubHandle::~SubHandle()
{
	if (m_subscription != nullptr && m_subscription->refct.dec())
	{
		m_sched->unsubscribe(m_subscription);
	}
}

bool Scheduler::SubHandle::setInterval(interval_t newDesiredInterval)
{
	if (m_subscription != nullptr)
	{
		m_sched->update(SubChange{ m_subscription->subid, m_subscription->type, newDesiredInterval, m_subscription->timescale });
		return true;
	}
	return false;
}

bool Scheduler::SubHandle::setSubscriptionType(SubType newDesiredType)
{
	if (m_subscription != nullptr)
	{
		m_sched->update(SubChange{ m_subscription->subid, newDesiredType, m_subscription->interval, m_subscription->timescale });
		return true;
	}
	return false;
}

bool Scheduler::SubHandle::setTimeScale(float newDesiredTimescale)
{
	if (m_subscription != nullptr)
	{
		m_sched->update(SubChange{ m_subscription->subid, m_subscription->type, m_subscription->interval, newDesiredTimescale });
		return true;
	}
	return false;
}

bool Scheduler::SubHandle::update(interval_t newDesiredInterval, SubType newDesiredType, float newDesiredTimescale)
{
	if (m_subscription != nullptr)
	{
		m_sched->update(SubChange{ m_subscription->subid, newDesiredType, newDesiredInterval, newDesiredTimescale });
		return true;
	}
	return false;
}

bool Scheduler::SubHandle::unsubscribe() //?
{
	if (m_subscription != nullptr && m_subscription->refct.dec())
	{
		m_sched->unsubscribe(m_subscription);
		m_subscription = nullptr;
		return true;
	}
	return false;
}

Scheduler::SchedSub::SchedSub() :
	subid(0),
	task(),
	type(SubType::Invalid),
	interval(0),
	timescale(1.0f),
	refct(0)
{
}

Scheduler::SchedSub::SchedSub(
	size_t _subid,
	TaskHandle && _task,
	SubType _type,
	interval_t _interval,
	float _timescale) :
	subid(_subid),
	task(std::move(_task)),
	type(_type),
	interval(_interval),
	timescale(_timescale),
	refct(0)
{
}