/** \addtogroup runtime
*  @{
*/

/*!
\file Scheduler.h
\brief Implements a rudimentary scheduler.
*/

#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_
#include <IPCore/ThreadingServices/TaskLib.h>
#include <vector>
#include <chrono>
#include <map>
#include <IPCore/Util/refctr.h>
#include <IPCore/Util/spinlock.h>
#include <IPCore/libs/concurrentqueue.h>
#include <IPCore/core_config.h>
#include <mutex>
#include <IPCore/RuntimeControl/Time.h>

namespace ipengine {

	/*!
	\brief A basic scheduler implementation.

	Modules that have to do work frequently can subscribe to the Scheduler.
	They pass a task function to be executed either per frame (core tick)
	or per interval. In return they get a handle to their subscription which allows
	them to change subscription parameters or to cancel the subscription manually.
	If the handle is destroyed, the subscription is automatically cancelled.
	*/
	class CORE_API Scheduler
	{
	public:
		//! Type of the subscription.
		enum class CORE_API SubType
		{
			Frame, //Once Per Frame
			Interval, //Every x interval
			Invalid
		};
	private:

		using interval_t = time_t;
		using sched_time_t = time_t;	

		//! Used to safely process changes to subscriptions. Objects of this class are enqueued and
		// the changes are applied at a appropriate point in time.
		struct CORE_API SubChange
		{
			size_t subid;
			SubType type;
			interval_t interval;
			float timescale;
		};

		/*!
		\brief Represents a subscription.
		*/
		class SchedSub
		{
		public:
			SchedSub();
			SchedSub(size_t _subid, TaskHandle&& _task, SubType _type, interval_t _interval, float _timescale);

			SchedSub(const SchedSub& other) = default;
			SchedSub(SchedSub&& other) = default;

			size_t subid;			//! ID of the subscription
			TaskHandle task;		//! The task that should be executed
			SubType type;			//! Type of the subscription
			interval_t interval;	//! If type is intercal, the interval that the subscriptions task is called in.
			int64_t acc;			//! Used to accumulate time between executions.
			float timescale;		//! Scales the dt that is given to the tasks on execution.
			RefCtr refct;			//! Implements reference counting for SchedSub
			bool mainThreadOnly;	//! True, if the subscriptions task must be executed on the main thread.
		};
	public:
		/*!
		\brief Passed to the subscriptions' tasks.
		*/
		class CORE_API SchedInfo
		{
		public:
			Time dt; //! Time delta the simulation should advanced by.
		};
	public:
		/*!
		\brief Modules receive an intance of SubHandle for every subscription.

		When a module subcribes to the Scheduler, it receives a SubHandle.
		The SubHandle instance allows the module to change subcription parameters
		and to cancel the subscription manually.
		If the SubHandle is destroyed, the subscription is cancelled automatically.
		*/
		class CORE_API SubHandle
		{
		public:
			SubHandle(SchedSub* sub, Scheduler* sched);
			SubHandle();
			SubHandle(const SubHandle& other);
			SubHandle(SubHandle&& other);
			~SubHandle();

			/*!
			\brief Changes the interval of a subscription.

			\param[in] newDesiredInterval	The new interval in nanoseconds. Use the Time class for more convenience.
			\returns						Returns true if the underlying subscription is valid.
			*/
			bool setInterval(interval_t newDesiredInterval);
			/*!
			\brief Changes the subscription type.

			\param[in] newDesiredType		The new desired subscription type.
			\returns						Returns true if the underlying subscription is valid.
			*/
			bool setSubscriptionType(SubType newDesiredType);
			/*!
			\brief Changes the time scale a subscription.

			The time scale is multiplied with the delta time that is passed to the
			subscriptions' tasks. Was originally thought for slow motion like effects but is currently not in use.

			\param[in] newDesiredTimescale	The new time scale.
			\returns						Returns true if the underlying subscription is valid.
			*/
			bool setTimeScale(float newDesiredTimescale);
			/*!
			\brief Combines the three functions above to a single function call.
			*/
			bool update(interval_t newDesiredInterval, SubType type, float timescale);

			/*!
			\brief Cancels the subscription.

			\returns Returns true if the underlying subscription is valid.
			*/
			bool unsubscribe();

		private:
			//! The handled subscription
			SchedSub* m_subscription;
			//! The Scheduler instance the subscription came from
			Scheduler* m_sched;
		};
	private:
		sched_time_t m_curtime; //Time elapsed since scheduler startup in nanoseconds. Overflows every 292.47... years
		std::atomic<size_t> m_idgen;
		std::map<uint64_t, SchedSub*> m_subscriptions;
		std::vector<SchedSub*> m_scheduledPoolSubs;
		std::vector<SchedSub*> m_scheduledMainThreadSubs;
		moodycamel::ConcurrentQueue<SubChange, moodycamel::ConcurrentQueueDefaultTraits> m_changequeue;
		using sub_entry_t = std::pair<size_t, SchedSub*>;
		YieldingSpinLock<5000> m_sublock;

		//! Cancels a subscription
		bool unsubscribe(SchedSub* s);
		//! Enqueues subscription parameter changes
		void update(const SubChange& change);
		//! Applies equeued subscription parameter changes
		void applyChanges();
	
	public:
		//! Is called frequently from the core.
		void schedule();
		Scheduler();
		~Scheduler();
		/*!
		\brief Creates a new subscription

		\param[in] taskfunc				The function that should be executed when a subscription is triggered
		\param[in] desiredInterval		The time interval the subscription should be triggered in.
		\param[in] type					Per frame or per interval subscription
		\param[in] timescale			Factor which scales the delta time that is passed to the task function.
		\param[in] pool					Pointer to the ThreadPool instance that should handle the tasks.
		\param[in] mainThreadOnly		Pass true if the subscription must not be executed on the thread pool.
		*/
		SubHandle subscribe(const ipengine::function<void(TaskContext&)>& taskfunc, interval_t desiredInterval, SubType type, float timescale, ThreadPool* pool, bool mainThreadOnly = false);
	};

}
#endif
/** @}*/