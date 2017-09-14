#pragma once

//TODO: implement continuation passing!

#include <ThreadingServices/Task.h>
#include <ThreadingServices/TaskHandle.h>
#include <Util/Deque.h>
#include <random>
#include <vector>
#include <iostream>
#include <Memory/allocators.h>
#include <atomic>
#include <Util/LFDS.h>
#include <assert.h>
#include <Util/stack.h>
#include <concurrentqueue.h>
#include <string>
#include <core_config.h>

namespace ipengine {

	class CORE_API ThreadPool
	{
		using ws_queue = ipengine::LockFreeWSQueue<Task*>;
		friend class TaskHandle;
		//Worker class
	private:
		class alignas(TS_CACHE_LINE_SIZE)CORE_API Worker
		{
		public:
			Worker(ThreadPool* pool, size_t id);
			~Worker();
			void run();
			void start();
			void stop();

			ws_queue local_queue;
			std::thread m_thread;
			stack<Task*> m_waitstack;
			ThreadPool* m_pool;
			std::atomic<bool> m_runflag;
			size_t id;



		};
		//public interface
	public:
		ThreadPool(size_t nworkers);
		~ThreadPool();

		void startWorkers();
		void stopWorkers();

		bool submit(TaskHandle& handle);
		bool spawn(TaskHandle& handle, TaskContext* tcptr = nullptr);
		void wait(TaskHandle& handle, TaskContext* tcptr = nullptr);
		//void wait_recycle(TaskHandle& handle, TaskContext* tcptr = nullptr);

		TaskHandle createTask(const TaskFunction& func, const TaskContext& context);
		TaskHandle createTask(const TaskFunction& func, TaskContext&& context);
		TaskHandle createTask(TaskFunction&& func, const TaskContext& context);
		TaskHandle createTask(TaskFunction&& func, TaskContext&& context);
		TaskHandle createChild(const TaskFunction& func, const TaskContext& context, TaskHandle& parent);
		TaskHandle createChild(const TaskFunction& func, TaskContext&& context, TaskHandle& parent);
		TaskHandle createChild(TaskFunction&& func, const TaskContext& context, TaskHandle& parent);
		TaskHandle createChild(TaskFunction&& func, TaskContext&& context, TaskHandle& parent);
		bool addChild(TaskHandle& parent, TaskHandle& child);
		std::thread::id getCurrentWorkerId()
		{
			return std::this_thread::get_id();
		}


		bool isRunning()
		{
			return m_isrunning.load(std::memory_order_relaxed);
		}

	private:
		void execute(Task* task);
		void execute(Task* task, Worker* worker);
		void finalize(Task* task);
		void waitForTask(Task*, TaskContext* tcptr = nullptr);
		bool help(Worker* worker = nullptr);
		Task* trySteal(Worker* worker);
		Task* tryGetTask(Worker* worker);

		Worker* getWorkerByThreadID(std::thread::id id);

		Task* create(const TaskFunction& func, const TaskContext& context);
		Task* create(TaskFunction&& func, const TaskContext& context);
		Task* create(const TaskFunction& func, TaskContext&& context);
		Task* create(TaskFunction&& func, TaskContext&& context);
		void use(Task* task);
		void release(Task* task, bool forcerelease = false);
		void recycle(Task* task);


		//private data
		moodycamel::ConcurrentQueue<Task*, moodycamel::ConcurrentQueueDefaultTraits> m_globalWorkQueue;
		ipengine::Deque<Task*> m_helperqueue; //this thing bottlenecks helperthreads if there are many of them
		spinlock_stack<Task*> m_helperwaitstack; //same problem here. but the stack could be replaced with a lockfree version.
		std::vector<aligned_ptr<Worker>> m_workers;
		std::atomic<bool> m_isrunning;
		using TaskAlloc = ThreadSafeFreeList<TS_CACHE_LINE_SIZE, sizeof(Task), 4096>;
		const size_t TSIZE = sizeof(Task);

		std::random_device rd;
		std::default_random_engine eng;
		std::uniform_int_distribution<size_t> rnd;

		inline size_t randomWorkerIndex(size_t except)
		{
			size_t rn = rnd(eng);
			if (rn == except)
				rn = (rn + 1) % m_workers.size();
			return rn;
		}
	};

}

