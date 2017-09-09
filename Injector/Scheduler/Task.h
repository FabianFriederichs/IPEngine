#ifndef _TASK_H_
#define _TASK_H_
#include <functional>
#include <atomic>
#include <memory>
#include "TaskContext.h"
#include "function.h"


using TaskFunction = function<void(TaskContext&)>;

class alignas(128) Task
{
public:
	Task() {}
	~Task() {}

	//members
	TaskFunction m_func;
	TaskContext m_context;	
	std::atomic<size_t> m_unfinished;	//Number of childs + 1 for the task itself
	std::atomic<size_t> m_refct;	//How many thingies are using this task?	
	Task* m_parent;
};









#endif
