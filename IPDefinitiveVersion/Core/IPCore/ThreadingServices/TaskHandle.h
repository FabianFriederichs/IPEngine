#ifndef _TASK_HANDLE_H_
#define _TASK_HANDLE_H_
#include <memory>
#include <atomic>
#include <IPCore/core_config.h>

#include <IPCore/ThreadingServices/Common.h>

namespace ipengine {
	class ThreadPool;
	class Task;

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

		bool wait(WorkerToken wtok = WorkerToken());
		bool wait_recycle(WorkerToken wtok = WorkerToken());
		bool addChild(TaskHandle& child);
		bool addContinuation(TaskHandle& continuationTask);
		bool spawn(WorkerToken wtok = WorkerToken());
		bool submit();
		void execute();
		TaskContext* getContext();		
	private:
		TaskHandle(Task* task, ThreadPool* pool);

		Task* m_task;
		ThreadPool* m_pool;
		bool m_isinpool;
	};

}
#endif