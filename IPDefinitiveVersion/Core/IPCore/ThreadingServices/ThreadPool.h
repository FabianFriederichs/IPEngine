#pragma once

//TODO: implement continuation passing!
//TODO: fork this thingy and create version without slow main thread helper path.
//instead "busy" wait with the main thread and yield. This should increase throughput and remove a huge amount of
//complexity and some contention problems from the system.
//All of that under the assumption that the OS and runtime liraries implement a meaningful "yield" operation

#include <IPCore/Util/Deque.h>
#include <random>
#include <vector>
#include <iostream>
#include <IPCore/Memory/lowlevel_allocators.h>
#include <atomic>
#include <IPCore/Util/LFDS.h>
#include <assert.h>
#include <IPCore/Util/stack.h>
#include <IPCore/libs/concurrentqueue.h>
#include <string>
#include <IPCore/core_config.h>

#include <IPCore/ThreadingServices/Common.h>
#include <IPCore/ThreadingServices/Task.h>
#include <IPCore/ThreadingServices/TaskHandle.h>
#include <IPCore/ThreadingServices/TaskContext.h>


namespace ipengine
{
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
		bool spawn(TaskHandle& handle, WorkerToken wtok = WorkerToken());
		void wait(TaskHandle& handle, WorkerToken wtok = WorkerToken());
		//void wait_recycle(TaskHandle& handle, TaskContext* tcptr = nullptr);

		TaskHandle createTask(const TaskFunction& func, const TaskContext& context);
		TaskHandle createTask(const TaskFunction& func, TaskContext&& context);
		TaskHandle createTask(TaskFunction&& func, const TaskContext& context);
		TaskHandle createTask(TaskFunction&& func, TaskContext&& context);
		TaskHandle createEmpty();
		TaskHandle createChild(const TaskFunction& func, const TaskContext& context, TaskHandle& parent);
		TaskHandle createChild(const TaskFunction& func, TaskContext&& context, TaskHandle& parent);
		TaskHandle createChild(TaskFunction&& func, const TaskContext& context, TaskHandle& parent);
		TaskHandle createChild(TaskFunction&& func, TaskContext&& context, TaskHandle& parent);
		bool addChild(TaskHandle& parent, TaskHandle& child);
		bool addContinuation(TaskHandle& task, TaskHandle& continuationTask);
		void executeImmediate(TaskHandle& task);
		std::thread::id getCurrentWorkerId()
		{
			return std::this_thread::get_id();
		}


		bool isRunning()
		{
			return m_isrunning.load(std::memory_order_relaxed);
		}

	private:
		void execute(Task* task, Worker* worker);
		void finalize(Task* task, Worker* worker);
		void waitForTask(Task*, const WorkerToken& wtok);
		bool help(Worker* worker);
		Task* trySteal(Worker* worker);
		Task* tryGetTask(Worker* worker);

		Worker* getWorkerFromToken(const WorkerToken& tok);
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

