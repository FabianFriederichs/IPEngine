/** \addtogroup threading
*  @{
*/

/*!
\file Task.h
*/

#ifndef _TASK_H_
#define _TASK_H_
#include <functional>
#include <atomic>
#include <memory>
#include <IPCore/Util/function.h>

#include <IPCore/ThreadingServices/TaskContext.h>

namespace ipengine {

	/*!
	\brief Defines the function signature for every task function.
	*/
	using TaskFunction = ipengine::function<void(TaskContext&)>;
	/*!
	\brief Dummy function for dummy tasks.
	*/
	inline void empty_func(TaskContext& empty) {};

	/*!
	\brief This class contains all the information neccessary to execute a piece of work on the thread pool.

	The Task class is kept as compact as possible and is requried to be aligned to cache line boundaries to prevent
	false sharing.
	*/
	class alignas(TS_CACHE_LINE_SIZE) CORE_API Task
	{
	public:
		Task() {}
		~Task() {}

		//! Execution context. Contains parameters for the task function.
		TaskContext m_context;												//max 72
		//! The function to be executed.
		TaskFunction m_func;												//32															//max 72
		//! The task is finished if m_unfinished has dropped to 0. A unfinished task has +1 for itself and +1 for every child task.
		std::atomic<size_t> m_unfinished;	//Number of childs + 1 for the task itself	//8
		//! Is used to reference count the task object.
		std::atomic<size_t> m_refct;	//How many thingies are using this task?		//8
		//! Pointer to the parent task. If that pointer is not nullptr, the parent must be notified when this task finishes.
		Task* m_parent;																	//8

																						
		//! Number of continuations for this task. Continuations are pushed onto the workers local queue if this task is finished.
		std::atomic<size_t> m_contcount;									//8
		//! Continuation tasks. Maximum number allowed is 15.
		Task* m_continuations[15];											//15*8 = 120
	};

}


#endif
/** @}*/