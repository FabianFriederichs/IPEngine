#pragma once
#include <ThreadingServices/TaskLib.h>
#include <vector>
#include <chrono>
#include <map>
#include <Util/refctr.h>
#include <Util/spinlock.h>
#include <libs/concurrentqueue.h>
#include <core_config.h>
#include <mutex>

//High level scheduler interface.
//subscribe/unsubscribe function

namespace ipengine {


	class CORE_API Scheduler
	{
		using interval_t = int64_t;
		using sched_time_t = int64_t;

		enum class CORE_API SubType
		{
			Frame, //Once Per Frame
			Interval, //Every x interval
			Invalid
		};

		struct CORE_API SubChange
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
			float timescale;
			RefCtr refct;
		};

		class CORE_API SchedInfo
		{
		public:
			double dt;
		};

		class CORE_API SubHandle
		{
		public:
			SubHandle(SchedSub* sub, Scheduler* sched);
			SubHandle();
			SubHandle(const SubHandle& other);
			SubHandle(SubHandle&& other);
			~SubHandle();

			bool setInterval(interval_t newDesiredInterval);
			bool setSubscriptionType(SubType newDesiredType);
			bool setTimeScale(float newDesiredTimescale);
			bool update(interval_t newDesiredInterval, SubType type, float timescale);

			bool unsubscribe();

		private:
			SchedSub* m_subscription;
			Scheduler* m_sched;
		};

		sched_time_t m_curtime; //Time elapsed since scheduler startup in nanoseconds. Overflows every 292.47... years
		sched_time_t m_accum;
		std::atomic<size_t> m_idgen;
		std::map<uint64_t, SchedSub*> m_subscriptions;
		moodycamel::ConcurrentQueue<SubChange, moodycamel::ConcurrentQueueDefaultTraits> m_changequeue;
		using sub_entry_t = std::pair<size_t, SchedSub*>;
		YieldingSpinLock<5000> m_sublock;

		bool unsubscribe(SchedSub* s);
		void update(const SubChange& change);
		void applyChanges();
		void schedule();

	public:
		Scheduler();
		~Scheduler();

		SubHandle subscribe(const ipengine::function<void(TaskContext&)>&, interval_t desiredInterval, SubType type, float timescale, ThreadPool* pool); //Temporary last param for testing. use core interface later!
	};

}


