/** \addtogroup threading
*  @{
*/

/*!
\file TaskHandle.h
*/

#ifndef _TASK_HANDLE_H_
#define _TASK_HANDLE_H_
#include <memory>
#include <atomic>
#include <IPCore/core_config.h>
#include <IPCore/ThreadingServices/Common.h>

namespace ipengine {
	class ThreadPool;
	class Task;

	/*!
	\brief Implements a handle to a task created on the thread pool.

	This class does two things:
	\li It manages the lifetime of a task.
	\li It provides convenient access to some thread pool functionality.

	If the last TaskHandle that references a Task is destroyed, the Task object is destroyed.
	If a Task object finishes its execution on the thread pool and no TaskHandle references it,
	the Task is destroyed.
	*/
	class CORE_API TaskHandle
	{
		friend class ThreadPool;
	public:
		//! Constructor
		TaskHandle();
		//! Copy constructor
		TaskHandle(const TaskHandle& _other);
		//! Move constructor
		TaskHandle(TaskHandle&& _other);
		//! Destructor
		~TaskHandle();

		/*!
		\brief Returns true if the handle is valid.
		\returns Returns true if the handle is valid.
		*/
		inline bool isValid() const
		{
			return m_task != nullptr && m_pool != nullptr;
		}

		/*!
		\brief Waits for the completion of a task.

		If the task was not submitted or spawned onto the pool yet, false is returned.
		If wait is called from a worker thread, the thread helps by executing other tasks
		while waiting.
		If wait is called from a non-worker thread, the function busy-waits for completion of the task.

		If you call this function inside a task function, pass the WorkerToken from TaskContext as parameter,
		this speeds up execution internally.

		\param[in] wtok		Worker token if available (call TaskContext::getWorkerToken() if a task context is available !)
		\returns			Returns true if the task completed execution. Returns false if the task was not submitted or spawned yet.

		\attention			Waiting for tasks that were executed by non-worker threads or through execute() or non-worker spawns
		\attention			currently results in application hangs. Must be fixed in the future.
		*/
		bool wait(WorkerToken wtok = WorkerToken());
		/*!
		\brief Same as wait, but resets the unfinished count of the task so that it can be resubmitted or respawned.
		\param[in] wtok		Worker token if available.

		\attention			This does not work for tasks with children or continuations attached currently!
		\todo				Maybe remove this function altogether. Complicated to implement efficiently and only creates problems.
		*/
		bool wait_recycle(WorkerToken wtok = WorkerToken());
		/*!
		\brief Adds a child to the task.

		The task is finished if itself finished execution and all children finished, too.
		\param[in] child	A valid task handle whose task should become a child of this task.
		\returns			Returns true if both handles were valid and the operation finished successfully.	
		*/
		bool addChild(TaskHandle& child);
		/*!
		\brief Adds a continuation task to the task.

		Continuations are tasks that are immediately spawned after the host task has finished.
		The maximum number of continuations is 15, currently.

		\param[in] continuationTask		Task that should be added as continuation
		\returns						Returns true if the operation finished successfully, false if one of the handles
										were invalid or no more continuation slots are available.
		*/
		bool addContinuation(TaskHandle& continuationTask);
		/*!
		\brief Pushes a task onto the local work queue of the executing worker.

		This function should be called by task functions that want to achieve further parallelization.
		To avoid some internal overhead, pass the worker token from the task context as parameter.
		If the function is called from a non-worker thread the task is executed immediately on this thread.

		\param[in] wtok Worker token, if available

		\attention	Spawning from a non-worker thread currently works only for tasks with no continuations or children.
		\attention	Calling the function in this situation results in application hangs and undefined behaviour.
		*/
		bool spawn(WorkerToken wtok = WorkerToken());
		/*!
		\brief Pushes a task onto the global work queue.

		This function should be called to submit the initial tasks onto the thread pool.
		Fine grained parallelization should be done on the fly by spawn calls inside the task functions.

		\returns	Returns true if the task was submitted successfully. Returns false if the task is already in the pool.
		*/
		bool submit();
		/*!
		\brief Executes the task immediately on the current thread.
		\attention Currently only works for tasks without children and continuations. Don't call wait or wait_recycle on tasks executed with this function!
		*/
		void execute();
		/*!
		\brief Returns a pointer to the TaskContext of the task.
		*/
		TaskContext* getContext();		
	private:
		TaskHandle(Task* task, ThreadPool* pool);

		Task* m_task;
		ThreadPool* m_pool;
		bool m_isinpool;
	};

}
#endif
/** @}*/