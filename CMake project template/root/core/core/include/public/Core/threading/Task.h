#ifndef _TASK_H_
#define _TASK_H_
#include <functional>
#include <atomic>
#include <memory>
#include <core/util/function.h>

#include <core/threading/TaskContext.h>

namespace ipengine {

	using TaskFunction = ipengine::function<void(TaskContext&)>;
	inline void empty_func(TaskContext& empty) {};

	class alignas(128) CORE_API Task
	{
	public:
		Task() {}
		~Task() {}

		//members
		TaskContext m_context;												//max 72
		TaskFunction m_func;												//32															//max 72
		std::atomic<size_t> m_unfinished;	//Number of childs + 1 for the task itself	//8
		std::atomic<size_t> m_refct;	//How many thingies are using this task?		//8
		Task* m_parent;																	//8

																						//continuation stealing stuff
																						/*
																						option 1: std::vector<Task*> + atomic counter => arbitrary amount of continuations, but allocation and access overhead
																						option 2: Task*[15] + atomic counter => fast?
																						*/
		std::atomic<size_t> m_contcount;									//8
		Task* m_continuations[15];											//15*8 = 120
	};

}


#endif

