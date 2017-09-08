#pragma once
#include "TaskLib.h"
#include <vector>
#include <chrono>
#include <map>
#include "refctr.h"
#include "spinlock.h"
#include <concurrentqueue.h>
//High level scheduler interface.
//subscribe/unsubscribe function



class Scheduler
{
	
public:
	using interval_t = int64_t;
	using sched_time_t = int64_t;

	enum class SubType
	{
		Frame, //Once Per Frame
		Interval, //Every x interval
		Invalid
	};
private:
	struct SubChange
	{
		size_t subid;
		SubType type;
		interval_t interval;
		float timescale;
	};

	class SchedSub
	{
	public:
		SchedSub();
		SchedSub(size_t _subid, TaskHandle&& _task, SubType _type, interval_t _interval, float _timescale);

		SchedSub(const SchedSub& other) = default;
		SchedSub(SchedSub&& other) = default;

		size_t subid;
		TaskHandle task;
		SubType type;
		interval_t interval;
		int64_t acc;
		float timescale;
		RefCtr refct;
		interval_t lastDelta;
		sched_time_t lastSchedActivity;
	};
public:
	class SubHandle
	{
	public:
		SubHandle(SchedSub* sub, Scheduler* sched) :
			m_subscription(sub),
			m_sched(sched)
		{
			sub->refct.inc();
		}
		SubHandle() :
			m_subscription(nullptr),
			m_sched(nullptr)
		{
		}
		SubHandle(const SubHandle& other) :
			m_subscription(other.m_subscription),
			m_sched(other.m_sched)
		{
			if (m_subscription != nullptr)
				m_subscription->refct.inc();
		}
		SubHandle(SubHandle&& other) :
			m_subscription(other.m_subscription),
			m_sched(other.m_sched)
		{
			other.m_sched = nullptr;
			other.m_subscription = nullptr;
		}
		~SubHandle()
		{
			if (m_subscription != nullptr && m_subscription->refct.dec())
			{
				m_sched->unsubscribe(m_subscription);
			}
		}

		bool setInterval(interval_t newDesiredInterval);
		bool setSubscriptionType(SubType newDesiredType);
		bool setTimeScale(float newDesiredTimescale);
		bool update(interval_t newDesiredInterval, SubType type, float timescale);
		int getID() { return m_subscription->subid; }

		bool unsubscribe() //?
		{
			if (m_subscription != nullptr && m_subscription->refct.dec())
			{
				m_sched->unsubscribe(m_subscription);
				m_subscription = nullptr;
				return true;
			}
			return false;
		}

	private:
		SchedSub* m_subscription;
		Scheduler* m_sched;
	};
	class SchedInfo
	{
	public:
		interval_t dt;
	};

private:

	

	

	

	sched_time_t m_curtime; //Time elapsed since scheduler startup in nanoseconds. Overflows every 292.47... years
	sched_time_t m_accum;
	std::atomic<size_t> m_idgen;
	std::map<uint64_t, SchedSub*> m_subscriptions;
	moodycamel::ConcurrentQueue<SubChange, moodycamel::ConcurrentQueueDefaultTraits> m_changequeue;
	using sub_entry_t = std::pair<size_t, SchedSub*>;
	YieldingSpinLock<5000> m_sublock;
	
	bool unsubscribe(SchedSub* s)
	{
		std::unique_lock<YieldingSpinLock<5000>> lock(m_sublock);
		bool ret = m_subscriptions.erase(s->subid);
		lock.unlock();
		delete s;
		return ret;
	}
	void update(const SubChange& change);
	void applyChanges();
	
	ThreadPool m_pool;

public:

	Scheduler();
	~Scheduler();
	void schedule();
	SubHandle subscribe(const function<void(TaskContext&)>&, interval_t desiredInterval, SubType type, float timescale); //Temporary last param for testing. use core interface later!
	void start();
};


