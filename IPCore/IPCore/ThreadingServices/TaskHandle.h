#pragma once
#include <memory>
#include <atomic>
#include <IPCore/ThreadingServices/Task.h>
#include <IPCore/ThreadingServices/ThreadPool.h>
#include <IPCore/core_config.h>

namespace ipengine {
	class ThreadPool;
	class CORE_API TaskHandle
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
		bool addContinuation(TaskHandle& continuationTask);
		bool spawn(TaskContext* tcptr = nullptr);
		bool submit();
		void execute();
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

}

