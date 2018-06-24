/** \addtogroup threading
Implements multi-threading support.
*  @{
*/

/*!
\file ThreadPool.h
*/

#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

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
	/*!
	\brief This class implements a lightweight thread pool system.

	The implementation follows the basic concept of work stealing (See http://supertech.csail.mit.edu/papers/steal.pdf).
	
	The basic idea is the following:
	A set of worker threads is created that pull work from work queues.
	The thread pool maintains a global work queue which workers pull tasks from and execute them.
	In addidtion to that, each worker has a local work queue to reduce contention on the global queue
	when many workers are running in parallel.

	The global queue in this implementation is taken from: https://github.com/cameron314/concurrentqueue
	It is lock-free and therefore reduces expensive system calls and contention on locks.

	The local, lock-free workstealing queue is implemented after the Chase-Lev paper: http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.170.1097&rep=rep1&type=pdf

	There are two basic operations: submit and spawn.
	\li	submit pushes a task onto the global work queue
	\li spawn pushes a task onto the local work queue

	If a worker tries to get a task to work on it does that in the following order:
	\li Try to pop a task from the local work queue, if empty...
	\li Try to pop a task from the global work queue. If that is empty too, ...
	\li Try to steal a task from the back of the local work queue of another worker.

	Doing so, the work is automatically balanced between workers as long as there are enough tasks.

	Tasks are created and destroyed using the ThreadSafeFreeList implementation.

	Best efficiency and parallelism is achieved in the following way:
	\li Submit large 'main tasks' from non-worker threads
	\li Spawn tasks inside task functions to achieve fine-grained parallelism

	\todo		Fix or remove the recycle function.
	\todo		Fix codepaths for immediate execution without hangs.
	\todo		Hide implementation detail by using the pimpl idiom and restructuring some things.
	*/
	class CORE_API ThreadPool
	{
		using ws_queue = ipengine::LockFreeWSQueue<Task*>;
		friend class TaskHandle;
	private:
		/*!
		\brief Worker class.

		Holds a worker's thread and local queue and implements the worker's loop
		that executes tasks.

		Can be started and stopped.

		Aligned to a multiple of a cache line size to prevent false sharing on
		worker access.
		*/
		class alignas(TS_CACHE_LINE_SIZE) CORE_API Worker
		{
		public:
			//! Initialize the worker
			Worker(ThreadPool* pool, size_t id);
			//! Destructor
			~Worker();
			//! The worker's main loop
			void run();
			//! Starts the worker
			void start();
			//! Stops the worker
			void stop();

			//! Local work queue
			ws_queue local_queue;
			//! The thread object that runs the worker's main loop
			std::thread m_thread;
			//! Pointer to the owning thread pool instance
			ThreadPool* m_pool;
			//! Signals the worker to stop when set to false
			std::atomic<bool> m_runflag;
			//! Worker ID
			size_t id;
		};		

		//public interface
	public:
		/*!
		\brief Initializes the thread pool with n workers.
		\param[in] nworkers		How many workers to create
		*/
		ThreadPool(size_t nworkers);
		//! Destructor
		~ThreadPool();

		/*!
		\brief Starts all workers
		*/
		void startWorkers();
		/*!
		\brief Stops all workers
		*/
		void stopWorkers();

		/*!
		\brief Submit a task

		Pushes a task onto the global work queue. Use this function for submitting
		large 'Mothertasks' that spawn many small tasks to achieve the best performance.

		\param[in] handle	A valid task handle
		\returns			Returns true on success
		*/
		bool submit(TaskHandle& handle);

		/*!
		\brief Spawn a task

		Pushes a task onto the local work queue. Use this function inside task functions
		to spawn additional subtasks.

		\param[in] handle	A valid task handle
		\param[in] wtok		Worker token if available. You can get this token from the TaskContext parameter of the task function.
							It acts as hint for the thread pool and removes some internal overhead.
		*/
		bool spawn(TaskHandle& handle, WorkerToken wtok = WorkerToken());

		/*!
		\brief Wait for a task's completion

		Waits until a task and all its children are finished. If called from a non-worker thread, a busy wait is performed.
		When called from a worker thread, the worker goes on and works on additional tasks while waiting.
		If the task has attached continuations, those are spawned as soon as the task is finished.

		\param[in] handle	A valid task handle
		\param[in] wtok		Worker token if available. You can get this token from the TaskContext parameter of the task function.
		It acts as hint for the thread pool and removes some internal overhead.

		\attention Do not wait on tasks that were executed by spawn on a non-worker thread or by executeImmediate, currently
		\attention that leads to application hangs. It will be fixed in the future.
		*/
		void wait(TaskHandle& handle, WorkerToken wtok = WorkerToken());

		/*!
		\brief Creates a task from the given task function and task context.

		Creates a task.

		\param[in] func		TaskFunction to be executed.
		\param[in] context	Execution context that shall be passed to the task function.
		\returns	Returns a TaskHandle to the newly created task.
		*/
		TaskHandle createTask(const TaskFunction& func, const TaskContext& context);
		/*!
		\brief Creates a task from the given task function and task context.

		Creates a task.

		\param[in] func		TaskFunction to be executed.
		\param[in] context	Execution context that shall be passed to the task function.
		\returns	Returns a TaskHandle to the newly created task.
		*/
		TaskHandle createTask(const TaskFunction& func, TaskContext&& context);
		/*!
		\brief Creates a task from the given task function and task context.

		Creates a task.

		\param[in] func		TaskFunction to be executed.
		\param[in] context	Execution context that shall be passed to the task function.
		\returns	Returns a TaskHandle to the newly created task.
		*/
		TaskHandle createTask(TaskFunction&& func, const TaskContext& context);
		/*!
		\brief Creates a task from the given task function and task context.

		Creates a task.

		\param[in] func		TaskFunction to be executed.
		\param[in] context	Execution context that shall be passed to the task function.
		\returns	Returns a TaskHandle to the newly created task.
		*/
		TaskHandle createTask(TaskFunction&& func, TaskContext&& context);
		/*!
		\brief Creates an empty dummy task. Can be used to model various relationships between tasks.
		\returns	Returns a TaskHandle to the newly created task.
		*/
		TaskHandle createEmpty();
		/*!
		\brief Creates a task from a task function and a task context and adds it to the given parent-task's children.

		\param[in] func		TaskFunction to be executed.
		\param[in] context	Execution context that shall be passed to the task function.
		\param[in] parent	A valid parent task handle.
		\returns			Returns a TaskHandle to the newly created task.
		*/
		TaskHandle createChild(const TaskFunction& func, const TaskContext& context, TaskHandle& parent);
		/*!
		\brief Creates a task from a task function and a task context and adds it to the given parent-task's children.

		\param[in] func		TaskFunction to be executed.
		\param[in] context	Execution context that shall be passed to the task function.
		\param[in] parent	A valid parent task handle.
		\returns			Returns a TaskHandle to the newly created task.
		*/
		TaskHandle createChild(const TaskFunction& func, TaskContext&& context, TaskHandle& parent);
		/*!
		\brief Creates a task from a task function and a task context and adds it to the given parent-task's children.

		\param[in] func		TaskFunction to be executed.
		\param[in] context	Execution context that shall be passed to the task function.
		\param[in] parent	A valid parent task handle.
		\returns			Returns a TaskHandle to the newly created task.
		*/
		TaskHandle createChild(TaskFunction&& func, const TaskContext& context, TaskHandle& parent);
		/*!
		\brief Creates a task from a task function and a task context and adds it to the given parent-task's children.

		\param[in] func		TaskFunction to be executed.
		\param[in] context	Execution context that shall be passed to the task function.
		\param[in] parent	A valid parent task handle.
		\returns			Returns a TaskHandle to the newly created task.
		*/
		TaskHandle createChild(TaskFunction&& func, TaskContext&& context, TaskHandle& parent);
		/*!
		\brief Adds a the child-task to the parent-task's children.

		A task is considered as finished if itself and all its children have completed execution.

		\param[in] parent	Valid parent task handle
		\param[in] child	Valid child task handle
		\returns			Returns true on success
		*/
		bool addChild(TaskHandle& parent, TaskHandle& child);
		/*!
		\brief Attaches a continuation task to an existing task.

		Continuation tasks are spawned immediately after the host task has finished.
		The number of continuations is limited to 15 tasks.

		\param[in] task					Task to attach the continuation to.
		\param[in] continuationTask		Task to be executed after task has finished.
		\returns						Returns true on success. False is returned if any of the task handles were invalid or
										the maximum number of continuations was exceeded.
		*/
		bool addContinuation(TaskHandle& task, TaskHandle& continuationTask);
		/*!
		\brief Executes the task function of a task immediately on the current thread.

		\param[in] task		Task to be executed.

		\attention	Currently works only for tasks without children and continuations.
		\attention	Do not wait for a task that was executed with this function as that would cause application hangs.
					When the function returns, the task is considered finished.
		*/
		void executeImmediate(TaskHandle& task);

		/*!
		\brief Returns the current thread id.
		*/
		std::thread::id getCurrentWorkerId()
		{
			return std::this_thread::get_id();
		}

		/*!
		\brief Returns true if the workers are currently running.
		*/
		bool isRunning()
		{
			return m_isrunning.load(std::memory_order_relaxed);
		}

	private:
		//! Executes a task on the given worker
		void execute(Task* task, Worker* worker);
		//! Finalizes a task that was executed on the given worker
		void finalize(Task* task, Worker* worker);
		/*!
		\brief Waits for a task to complete.
		 
		If the function is called from a worker thread, other tasks are executed while waiting.
		If called from a non-worker thread, the function busy-waits for completion of the task.
		*/
		void waitForTask(Task*, const WorkerToken& wtok);
		/*!
		\brief Is called by waiting workers to execute other tasks while waiting.
		\returns Returns true if a task was executed.
		*/
		bool help(Worker* worker);
		/*!
		\brief Try to steal a task from the local work queue of a random worker (except worker).
		\param[in] worker The worker that tries to steal a task.
		\returns Returns a pointer to a task that waits for execution or nullptr if no task is available.
		*/
		Task* trySteal(Worker* worker);
		/*!
		\brief Called by workers to get the next available task.

		The following order is used to get a task:
		\li Try to pop a task from the local work queue, if empty...
		\li Try to pop a task from the global work queue. If that is empty too, ...
		\li Try to steal a task from the back of the local work queue of another worker.

		\param[in] worker The worker that tries to get a task.
		\returns Returns a pointer to a task that waits for execution or nullptr if no task is available.
		*/
		Task* tryGetTask(Worker* worker);

		//! Get a pointer to the worker from a worker token.
		Worker* getWorkerFromToken(const WorkerToken& tok);
		//! Get the worker that has a specific thread id.
		Worker* getWorkerByThreadID(std::thread::id id);

		//! Allocate and initialize a new task.
		Task* create(const TaskFunction& func, const TaskContext& context);
		//! Allocate and initialize a new task.
		Task* create(TaskFunction&& func, const TaskContext& context);
		//! Allocate and initialize a new task.
		Task* create(const TaskFunction& func, TaskContext&& context);
		//! Allocate and initialize a new task.
		Task* create(TaskFunction&& func, TaskContext&& context);
		//! Increase the task's reference count.
		void use(Task* task);
		//! Decrease the task's reference count. If the reference count reaches 0, destroy and deallocate the task.
		void release(Task* task, bool forcerelease = false);
		/*!
		\brief Reset the unfinished count of a task to 1.
		\attention Only makes sense for tasks with no children and no continuations. Undefined behaviour otherwise.
		*/
		void recycle(Task* task);


		//private data
		//! Global work queue
		moodycamel::ConcurrentQueue<Task*, moodycamel::ConcurrentQueueDefaultTraits> m_globalWorkQueue;
		//! Helper queue (not used anymore)
		ipengine::Deque<Task*> m_helperqueue; //this thing bottlenecks helperthreads if there are many of them
		//! Worker instances. Aligned to cache line boundaries to prevent false sharing.
		std::vector<aligned_ptr<Worker>> m_workers;
		//! Flag that indicates that the workers are running.
		std::atomic<bool> m_isrunning;
		//! Typedef for the ThreadSafeFreeList template instatiation used to allocate and deallocate tasks.
		using TaskAlloc = ThreadSafeFreeList<TS_CACHE_LINE_SIZE, sizeof(Task), 4096>;
		const size_t TSIZE = sizeof(Task);

		//! Used to select a random worker
		std::random_device rd;
		//! Used to select a random worker
		std::default_random_engine eng;
		//! Used to select a random worker
		std::uniform_int_distribution<size_t> rnd;

		//! Select a random worker index.
		inline size_t randomWorkerIndex(size_t except)
		{
			size_t rn = rnd(eng);
			if (rn == except)
				rn = (rn + 1) % m_workers.size();
			return rn;
		}
	};

}
#endif
/** @}*/