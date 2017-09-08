#include "ThreadPool.h"

// ----------------------------------------------- TASK SYSTEM INTERNAL SECTION --------------------------------------------------------

ThreadPool::Worker::Worker(ThreadPool * pool, size_t id) :
	m_pool(pool),
	local_queue(),
	id(id)
{
	if (m_pool == nullptr)
		throw std::invalid_argument("Class Worker, function Worker(ThreadPool * pool): Thread pool for worker creation is nullptr.");
	//m_waitstack.reserve(4096);
}

ThreadPool::Worker::~Worker()
{
}

void ThreadPool::Worker::run()
{
	while (m_runflag.load(std::memory_order_relaxed))
	{
		//TODO: let the worker sleep when nothing happens
		Task* task = m_pool->tryGetTask(this);
		if (task != nullptr)
		{
			m_pool->execute(task, this);
			//m_pool->finalize(task);
		}
		else
		{
			std::this_thread::yield();
		}
	}
}

void ThreadPool::Worker::start()
{
	m_runflag.store(true, std::memory_order_relaxed);
	m_thread = std::move(std::thread(&ThreadPool::Worker::run, this));
	std::cout << "Started worker thread: " << m_thread.get_id() << "\n";
}

void ThreadPool::Worker::stop()
{
	m_runflag.store(false, std::memory_order_relaxed);
	m_thread.join();
}

void ThreadPool::execute(Task * task)
{
	assert(task->m_unfinished <= 1);
	//assert(task != nullptr && task->m_refct.load() > 0 && task->m_unfinished > 0);
	Task* current;
	size_t cursize = m_helperwaitstack.size();
	m_helperwaitstack.push(task);
	while (m_helperwaitstack.size() > cursize)
	{

		if (m_helperwaitstack.top()->m_unfinished.load(std::memory_order_acquire) > 1)
		{
			current = tryGetTask(nullptr);
			if (current != nullptr)
			{
				m_helperwaitstack.push(current);
			}
			else
			{
				std::this_thread::yield();
			}
		}
		else
		{
			current = m_helperwaitstack.pop();
			try
			{
				current->m_context.workerid = -1;
				current->m_context.pool = this;
				current->m_func(current->m_context);
				finalize(current);
			}
			catch (std::exception& ex)
			{
				size_t exlth = std::char_traits<char>::length(ex.what());
				char* ex_msg = new char[exlth];
				strncpy_s(ex_msg, exlth, ex.what(), exlth);
				current->m_context.ex.reset(ex_msg);				//store exceptions inside task context for later rethrowing
			}
			catch (...)
			{
				size_t exlth = std::char_traits<char>::length("Unknown exception");
				char* ex_msg = new char[exlth];
				strncpy_s(ex_msg, exlth, "Unknown exception", exlth);
				current->m_context.ex.reset(ex_msg);
			}
		}
	}
	


	//try
	//{
	//	while (task->m_unfinished.load(std::memory_order_acquire) > 1)
	//	{
	//		help();	//TODO: make this stuff iterative to prevent stack overflow
	//	}
	//	task->m_func(task->m_context);
	//	finalize(task);
	//}
	//catch (const std::exception& ex)
	//{
	//	task->m_context.ex = std::move(ex);				//store exceptions inside task context for later rethrowing
	//}
	//catch (...)
	//{
	//	task->m_context.ex = std::logic_error("Something bad happened.");
	//}
}

void ThreadPool::execute(Task * task, Worker* worker)
{
	assert(task->m_unfinished <= 1);
	//assert(task != nullptr && task->m_refct.load() > 0 && task->m_unfinished > 0);
	Task* current;
	size_t cursize = worker->m_waitstack.size();
	worker->m_waitstack.push(task);
	while (worker->m_waitstack.size() > cursize)
	{			
		if (worker->m_waitstack.top()->m_unfinished.load(std::memory_order_acquire) > 1)
		{
			current = tryGetTask(worker);
			if (current != nullptr)
			{
				worker->m_waitstack.push(current);
			}
			else
			{
				std::this_thread::yield();
			}
		}
		else
		{
			current = worker->m_waitstack.pop();
			try
			{
				current->m_context.workerid = static_cast<int>(worker->id);
				current->m_context.pool = this;
				current->m_func(current->m_context);
				finalize(current);
			}
			catch (std::exception& ex)
			{
				size_t exlth = std::char_traits<char>::length(ex.what());
				char* ex_msg = new char[exlth];
				strncpy_s(ex_msg, exlth, ex.what(), exlth);
				current->m_context.ex.reset(ex_msg);				//store exceptions inside task context for later rethrowing
			}
			catch (...)
			{
				size_t exlth = std::char_traits<char>::length("Unknown exception");
				char* ex_msg = new char[exlth];
				strncpy_s(ex_msg, exlth, "Unknown exception", exlth);
				current->m_context.ex.reset(ex_msg);
			}
		}
	}	

	//try
	//{
	//	while (task->m_unfinished.load(std::memory_order_acquire) > 1)
	//	{
	//		Task* otask = tryGetTask(worker);
	//		if (otask != nullptr)
	//		{
	//			execute(task, worker);	//TODO: make this stuff iterative to prevent stack overflow				
	//		}
	//		else
	//		{
	//			std::this_thread::yield();
	//		}
	//	}
	//	task->m_func(task->m_context);		//TODO: explicit waiting for children?
	//	finalize(task);
	//}
	//catch (const std::exception& ex)
	//{
	//	task->m_context.ex = std::move(ex);				//store exceptions inside task context for later rethrowing
	//}
	//catch (...)
	//{
	//	task->m_context.ex = std::logic_error("Something bad happened.");
	//}
}

void ThreadPool::finalize(Task * task)
{
	/*if (task->m_unfinished.fetch_sub(1, std::memory_order_acq_rel) == 1)
	{
		if (task->m_parent)
		{
			finalize(task->m_parent);
		}
		release(task);
	}*/

	Task* c = task;
	Task* r;

	while (c != nullptr && c->m_unfinished.fetch_sub(1, std::memory_order_acq_rel) == 1)
	{
		r = c;
		c = c->m_parent;
		release(r);
	}
}

Task * ThreadPool::tryGetTask(Worker * worker)
{
	Task* task = nullptr;
	bool sc;

	//try pop from local queue
	if (worker != nullptr)
	{
		sc = worker->local_queue.pop(task);

		if (sc && task != nullptr)
		{
			//std::cout << "LOCAL TASK " << std::this_thread::get_id() << "\n";
		//	assert(task->m_unfinished.load() > 0);
			return task;
		}
	}
	else //non worker helpers
	{
		sc = m_helperqueue.try_pop_left(task);
		if (sc && task != nullptr)
		{
			//assert(task->m_unfinished.load() > 0);
			return task;
		}
	}
	

	//try pop from global queue
	//sc = m_globalWorkQueue.try_pop_left(task);
	sc = m_globalWorkQueue.try_dequeue(task);
	if (sc && task != nullptr)
	{
		//std::cout << "GLOBAL TASK " << std::this_thread::get_id() << "\n";
	//	assert(task->m_unfinished.load() > 0);
		return task;
	}


	//try steal
	task = trySteal(worker);
	if (task != nullptr)
	{
		//std::cout << "STOLEN TASK " << std::this_thread::get_id() << "\n";
		return task;
	}

	return nullptr;
}

void ThreadPool::waitForTask(Task * task, TaskContext* tcptr)
{	//TODO: state change?
	assert(task->m_unfinished <= 1);
	while (task->m_unfinished.load(std::memory_order_acquire) > 0)
	{
		if (!help((tcptr && tcptr->workerid > -1 ? m_workers[tcptr->workerid].get() : nullptr)))
			std::this_thread::yield();
		//help();
	}
 }



bool ThreadPool::help(Worker* _worker)
{
	Worker* worker = (_worker != nullptr ? _worker : getWorkerByThreadID(std::this_thread::get_id()));
	
	Task * task = tryGetTask(worker);
	if (task != nullptr)
	{
		if (worker != nullptr)
			execute(task, worker);
		else
			execute(task);
		//finalize(task);
		return true;
	}
	else
	{
		return false;
	}
}

Task * ThreadPool::trySteal(Worker* worker)
{
	size_t wid;
	if (worker != nullptr)
		wid = worker->id;
	else
		wid = m_workers.size();
	size_t rwid = randomWorkerIndex(wid);
	Task* task = nullptr;
	bool sc = m_workers[rwid]->local_queue.steal(task);
	if (sc && task != nullptr)
	{
	//	assert(task->m_unfinished.load() > 0);
		return task;
	}
	else if(worker != nullptr)
	{
		m_helperqueue.try_pop_right(task);
		if (task != nullptr)
		{
	//		assert(task->m_unfinished.load() > 0);
			return task;
		}
	}
	return nullptr;
}

Task * ThreadPool::create(const TaskFunction & func, const TaskContext & context)
{
	//Task * task = new Task();	//allocate from pool later
	Task* task = new(TaskAlloc::allocate(TSIZE))Task;
	task->m_func = func;
	task->m_context = context;
	task->m_parent = nullptr;
	task->m_refct.store(1, std::memory_order_relaxed);
	task->m_unfinished.store(1, std::memory_order_release);	
	assert(task->m_unfinished <= 1);
	return task;
}

Task * ThreadPool::create(TaskFunction && func, const TaskContext & context)
{
	//Task * task = new Task();	//allocate from pool later
	Task* task = new(TaskAlloc::allocate(TSIZE))Task;
	task->m_func = std::move(func);
	task->m_context = context;
	task->m_parent = nullptr;
	task->m_refct.store(1, std::memory_order_relaxed);
	task->m_unfinished.store(1, std::memory_order_release);
	assert(task->m_unfinished <= 1);
	return task;
}

Task * ThreadPool::create(const TaskFunction & func, TaskContext && context)
{
	//Task * task = new Task();	//allocate from pool later
	Task* task = new(TaskAlloc::allocate(TSIZE))Task;
	task->m_func = func;
	task->m_context = std::move(context);
	task->m_parent = nullptr;
	task->m_refct.store(1, std::memory_order_relaxed);
	task->m_unfinished.store(1, std::memory_order_release);
	assert(task->m_unfinished <= 1);
	return task;
}

Task * ThreadPool::create(TaskFunction && func, TaskContext && context)
{
	//Task * task = new Task();	//allocate from pool later
	Task* task = new(TaskAlloc::allocate(TSIZE))Task;
	task->m_func = std::move(func);
	task->m_context = std::move(context);
	task->m_parent = nullptr;
	task->m_refct.store(1, std::memory_order_relaxed);
	task->m_unfinished.store(1, std::memory_order_release);
	assert(task->m_unfinished <= 1);
	return task;
}

void ThreadPool::use(Task * task)
{
	task->m_refct.fetch_add(1, std::memory_order_relaxed);
}

void ThreadPool::release(Task * task, bool forcerelease)
{
	//if ((task->m_refct.load() == 0 || task->m_refct.fetch_sub(1) == 1) && (task->m_unfinished.load() == 0 || forcerelease))
	//{
	//	/*if (task->m_parent)
	//		task->m_parent->m_refct.fetch_sub(1);*/
	//	//delete task;
	//	TaskAlloc::deallocate(task);
	//}

	if (task->m_refct.fetch_sub(1, std::memory_order_release) == 1)
	{
		std::atomic_thread_fence(std::memory_order_acquire);
		//task->isdead.store(true);
		task->~Task();
		TaskAlloc::deallocate(task);
	}
}

void ThreadPool::recycle(Task * task)
{
	task->m_unfinished.store(1, std::memory_order_relaxed);
}

ThreadPool::Worker * ThreadPool::getWorkerByThreadID(std::thread::id id)
{
	for (size_t i = 0; i < m_workers.size(); i++)
		if (m_workers[i]->m_thread.get_id() == id) return m_workers[i].get();
	return nullptr;
}

// ---------------------------------------- TASK SYSTEM EXTERNAL SECTION ------------------------------------------------------------------

ThreadPool::ThreadPool(size_t nworkers) :
	rd(),
	eng(rd()),
	rnd(0, nworkers - 1),
	m_helperqueue()
{
	for (size_t i = 0; i < nworkers; i++)
	{
		m_workers.push_back(alloc_aligned<ThreadPool::Worker, CACHE_LINE_SIZE>(this, i));
	}
	m_isrunning.store(false, std::memory_order_release);
}

ThreadPool::~ThreadPool()
{
	for (size_t i = 0; i < m_workers.size(); i++)
	{
		free_aligned(m_workers[i]);
	}
}

void ThreadPool::startWorkers()
{
	for (size_t i = 0; i < m_workers.size(); i++)
	{
		m_workers[i]->start();
	}
	m_isrunning.store(true, std::memory_order_release);
}

void ThreadPool::stopWorkers()
{
	m_isrunning.store(false, std::memory_order_release);
	for (size_t i = 0; i < m_workers.size(); i++)
	{
		m_workers[i]->stop();
	}
}

bool ThreadPool::submit(TaskHandle& handle)
{
	if (handle.isValid() && m_isrunning.load(std::memory_order_acquire))
	{
		use(handle.m_task);
		//m_globalWorkQueue.push_right(handle.m_task);
		m_globalWorkQueue.enqueue(handle.m_task);
		
		return true;
	}
	return false;
}

bool ThreadPool::spawn(TaskHandle& handle, TaskContext* tcptr)
{
	if (handle.isValid() && m_isrunning.load(std::memory_order_acquire))
	{
		Worker* w = (tcptr && tcptr->workerid > -1 ? m_workers[tcptr->workerid].get() : getWorkerByThreadID(std::this_thread::get_id())); //TODO: Fix this
		if (w != nullptr)
		{
			use(handle.m_task);
		//	assert(handle.m_task->m_unfinished.load() != 0);// && handle.m_task->isdead.load() == false);
			w->local_queue.push(handle.m_task);				
			return true;
		}
		else
		{
			use(handle.m_task);
		//	assert(handle.m_task->m_unfinished.load() != 0);// && handle.m_task->isdead.load() == false);
			m_helperqueue.push_left(handle.m_task);					//and that
			return true;
		}
		//TODO: spawning a task from a non-worker thread?
	}
	return false;
}

TaskHandle ThreadPool::createTask(const TaskFunction & func, const TaskContext & context)
{
	Task* t = create(func, context);
	return TaskHandle(t, this);
}

TaskHandle ThreadPool::createTask(const TaskFunction & func, TaskContext && context)
{
	Task* t = create(func, std::move(context));
	return TaskHandle(t, this);
	return TaskHandle();
}

TaskHandle ThreadPool::createTask(TaskFunction && func, const TaskContext & context)
{
	Task* t = create(std::move(func), context);
	return TaskHandle(t, this);
	return TaskHandle();
}

TaskHandle ThreadPool::createTask(TaskFunction && func, TaskContext && context)
{
	Task* t = create(std::move(func), std::move(context));
	return TaskHandle(t, this);
	return TaskHandle();
}

TaskHandle ThreadPool::createChild(const TaskFunction & func, const TaskContext & context, TaskHandle& parent)
{
	if (parent.isValid())
	{
		Task* t = create(func, context);
		if (t == nullptr)
			return TaskHandle();

		//update parent unfinished count via CAS loop. If parents unfinished count becomes 0 we return a invalid task handle
		size_t unf = parent.m_task->m_unfinished.load(std::memory_order_relaxed);
		do
		{
			if (unf == 0)
				return TaskHandle();
		}
		while(!parent.m_task->m_unfinished.compare_exchange_weak(unf, unf + 1, std::memory_order_release, std::memory_order_relaxed));

		//if cas loop was successful, parent can't be finished until this child is. Set parent of our new child.
		t->m_parent = parent.m_task;		
		return TaskHandle(t, this);
	}	
	return TaskHandle();
}

TaskHandle ThreadPool::createChild(const TaskFunction & func, TaskContext && context, TaskHandle & parent)
{
	if (parent.isValid())
	{
		Task* t = create(func, std::move(context));
		if (t == nullptr)
			return TaskHandle();

		//update parent unfinished count via CAS loop. If parents unfinished count becomes 0 we return a invalid task handle
		size_t unf = parent.m_task->m_unfinished.load(std::memory_order_relaxed);
		do
		{
			if (unf == 0)
				return TaskHandle();
		} while (!parent.m_task->m_unfinished.compare_exchange_weak(unf, unf + 1, std::memory_order_release, std::memory_order_relaxed));

		//if cas loop was successful, parent can't be finished until this child is. Set parent of our new child.
		t->m_parent = parent.m_task;
		return TaskHandle(t, this);
	}
	return TaskHandle();
}

TaskHandle ThreadPool::createChild(TaskFunction && func, const TaskContext & context, TaskHandle & parent)
{
	if (parent.isValid())
	{
		Task* t = create(std::move(func), context);
		if (t == nullptr)
			return TaskHandle();

		//update parent unfinished count via CAS loop. If parents unfinished count becomes 0 we return a invalid task handle
		size_t unf = parent.m_task->m_unfinished.load(std::memory_order_relaxed);
		do
		{
			if (unf == 0)
				return TaskHandle();
		} while (!parent.m_task->m_unfinished.compare_exchange_weak(unf, unf + 1, std::memory_order_release, std::memory_order_relaxed));

		//if cas loop was successful, parent can't be finished until this child is. Set parent of our new child.
		t->m_parent = parent.m_task;
		return TaskHandle(t, this);
	}
	return TaskHandle();
}

TaskHandle ThreadPool::createChild(TaskFunction && func, TaskContext && context, TaskHandle & parent)
{
	if (parent.isValid())
	{
		Task* t = create(std::move(func), std::move(context));
		if (t == nullptr)
			return TaskHandle();

		//update parent unfinished count via CAS loop. If parents unfinished count becomes 0 we return a invalid task handle
		size_t unf = parent.m_task->m_unfinished.load(std::memory_order_relaxed);
		do
		{
			if (unf == 0)
				return TaskHandle();
		} while (!parent.m_task->m_unfinished.compare_exchange_weak(unf, unf + 1, std::memory_order_release, std::memory_order_relaxed));

		//if cas loop was successful, parent can't be finished until this child is. Set parent of our new child.
		t->m_parent = parent.m_task;
		return TaskHandle(t, this);
	}
	return TaskHandle();
}

bool ThreadPool::addChild(TaskHandle & parent, TaskHandle & child)
{
	if (parent.isValid() && child.isValid())
	{
		if (parent.m_task == child.m_task->m_parent)
			return true;
		//update parent unfinished count via CAS loop. If parents unfinished count becomes 0 we return false
		size_t unf = parent.m_task->m_unfinished.load(std::memory_order_relaxed);
		do
		{
			if (unf == 0)
				return false;
		} while (!parent.m_task->m_unfinished.compare_exchange_weak(unf, unf + 1, std::memory_order_release, std::memory_order_relaxed));

		//if cas loop was successful, parent can't be finished until this child is. Set parent of our new child.
		child.m_task->m_parent = parent.m_task;
	}
	return false;
}

void ThreadPool::wait(TaskHandle & handle, TaskContext* tcptr)
{
	if (handle.isValid())
		waitForTask(handle.m_task, tcptr);
}


