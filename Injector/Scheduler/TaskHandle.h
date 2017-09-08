#pragma once
#include <memory>
#include <atomic>
#include "Task.h"
#include "ThreadPool.h"
//TODO: throw exception if there is one in task context
class ThreadPool;
class TaskHandle
{
	friend class ThreadPool;
public:
	TaskHandle();	
	TaskHandle(const TaskHandle& _other);
	TaskHandle(TaskHandle&& _other);
	~TaskHandle();

	inline bool isValid() const
	{
		return m_task != nullptr && m_pool != nullptr;
	}

	bool wait(TaskContext* tcptr = nullptr);
	bool wait_recycle(TaskContext* tcptr = nullptr);
	bool addChild(TaskHandle& child);
	bool spawn(TaskContext* tcptr = nullptr);
	bool submit();
	TaskContext* getContext()
	{
		if (isValid())
			return &m_task->m_context;
		else
			return nullptr;
	}
private:
	TaskHandle(Task* task, ThreadPool* pool);

	Task* m_task;
	ThreadPool* m_pool;
	bool m_isinpool;
};

