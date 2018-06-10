#ifndef _TS_COMMON_H_
#define _TS_COMMON_H_

namespace ipengine
{
	class WorkerToken
	{
		friend class ThreadPool;
		friend class TaskContext;
	public:
		WorkerToken() :
			worker_id(-1)
		{}
		WorkerToken(const WorkerToken& other) = default;
		WorkerToken& operator=(const WorkerToken& other) = default;
		~WorkerToken() = default;
	private:
		WorkerToken(int wid) :
			worker_id(wid)
		{}
		int worker_id;
	};
}

#endif