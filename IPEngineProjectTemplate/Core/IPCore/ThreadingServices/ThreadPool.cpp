#include <IPCore/ThreadingServices/ThreadPool.h>

// ----------------------------------------------- TASK SYSTEM INTERNAL SECTION --------------------------------------------------------
ipengine::ThreadPool::Worker::Worker(ipengine::ThreadPool * pool, size_t id) :
	m_pool(pool),
	local_queue(),
	id(id)
{
	if (m_pool == nullptr)
		throw std::invalid_argument("Class Worker, function Worker(ThreadPool * pool): Thread pool for worker creation is nullptr.");
	//m_waitstack.reserve(4096);
}

ipengine::ThreadPool::Worker::~Worker()
{}

void ipengine::ThreadPool::Worker::run()
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

void ipengine::ThreadPool::Worker::start()
{
	m_runflag.store(true, std::memory_order_relaxed);
	m_thread = std::move(std::thread(&ThreadPool::Worker::run, this));
	std::cout << "Started worker thread: " << m_thread.get_id() << "\n";
}

void ipengine::ThreadPool::Worker::stop()
{
	m_runflag.store(false, std::memory_order_relaxed);
	m_thread.join();
}

void ipengine::ThreadPool::execute(Task * task)
{
	//assert(task != nullptr && task->m_refct.load() > 0 && task->m_unfinished > 0);
	try
	{
		task->m_context.workerid = -1;
		task->m_context.pool = this;
		task->m_func(task->m_context);
		finalize(task, nullptr);
	}
	catch (std::exception& ex)
	{
		size_t exlth = std::char_traits<char>::length(ex.what());
		char* ex_msg = new char[exlth];
		strncpy_s(ex_msg, exlth, ex.what(), exlth);
		task->m_context.ex.reset(ex_msg);				//store exceptions inside task context for later rethrowing
	}
	catch (...)
	{
		size_t exlth = std::char_traits<char>::length("Unknown exception");
		char* ex_msg = new char[exlth];
		strncpy_s(ex_msg, exlth, "Unknown exception", exlth);
		task->m_context.ex.reset(ex_msg);
	}
}

void ipengine::ThreadPool::execute(Task * task, Worker* worker)
{
	try
	{
		task->m_context.workerid = static_cast<int>(worker->id);
		task->m_context.pool = this;
		task->m_func(task->m_context);
		finalize(task, worker);
	}
	catch (std::exception& ex)
	{
		size_t exlth = std::char_traits<char>::length(ex.what());
		char* ex_msg = new char[exlth];
		strncpy_s(ex_msg, exlth, ex.what(), exlth);
		task->m_context.ex.reset(ex_msg);				//store exceptions inside task context for later rethrowing
	}
	catch (...)
	{
		size_t exlth = std::char_traits<char>::length("Unknown exception");
		char* ex_msg = new char[exlth];
		strncpy_s(ex_msg, exlth, "Unknown exception", exlth);
		task->m_context.ex.reset(ex_msg);
	}
}

void ipengine::ThreadPool::finalize(Task * task, Worker* worker)
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

		for (size_t i = 0; i < r->m_contcount.load(std::memory_order_acquire); i++)
		{
			if (worker != nullptr)
			{
				worker->local_queue.push(r->m_continuations[i]);
			}
			else
			{
				m_helperqueue.push_left(r->m_continuations[i]);
			}
		}

		release(r);
	}
}

ipengine::Task * ipengine::ThreadPool::tryGetTask(Worker * worker)
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
	sc = m_globalWorkQueue.try_dequeue(task);
	if (sc && task != nullptr)
	{
		return task;
	}


	//try steal
	task = trySteal(worker);
	if (task != nullptr)
	{
		return task;
	}

	return nullptr;
}

void ipengine::ThreadPool::waitForTask(Task * task, TaskContext* tcptr)
{
	while (task->m_unfinished.load(std::memory_order_acquire) > 0)
	{
		if (!help((tcptr && tcptr->workerid > -1 ? m_workers[tcptr->workerid].get() : nullptr)))
			std::this_thread::yield();
	}
}



bool ipengine::ThreadPool::help(Worker* _worker)
{
	Worker* worker = (_worker != nullptr ? _worker : getWorkerByThreadID(std::this_thread::get_id()));

	Task * task = tryGetTask(worker);
	if (task != nullptr)
	{
		if (worker != nullptr)
			execute(task, worker);
		else
			execute(task);
		return true;
	}
	else
	{
		return false;
	}
}

ipengine::Task * ipengine::ThreadPool::trySteal(Worker* worker)
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
	else if (worker != nullptr)
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

ipengine::Task * ipengine::ThreadPool::create(const TaskFunction & func, const TaskContext & context)
{
	//Task * task = new Task();	//allocate from pool later
	Task* task = new(TaskAlloc::allocate(TSIZE))Task;
	task->m_func = func;
	task->m_context = context;
	task->m_parent = nullptr;
	task->m_contcount.store(0, std::memory_order_relaxed);
	task->m_refct.store(1, std::memory_order_relaxed);
	task->m_unfinished.store(1, std::memory_order_release);
	return task;
}

ipengine::Task * ipengine::ThreadPool::create(TaskFunction && func, const TaskContext & context)
{
	//Task * task = new Task();	//allocate from pool later
	Task* task = new(TaskAlloc::allocate(TSIZE))Task;
	task->m_func = std::move(func);
	task->m_context = context;
	task->m_parent = nullptr;
	task->m_contcount.store(0, std::memory_order_relaxed);
	task->m_refct.store(1, std::memory_order_relaxed);
	task->m_unfinished.store(1, std::memory_order_release);
	return task;
}

ipengine::Task * ipengine::ThreadPool::create(const TaskFunction & func, TaskContext && context)
{
	//Task * task = new Task();	//allocate from pool later
	Task* task = new(TaskAlloc::allocate(TSIZE))Task;
	task->m_func = func;
	task->m_context = std::move(context);
	task->m_parent = nullptr;
	task->m_contcount.store(0, std::memory_order_relaxed);
	task->m_refct.store(1, std::memory_order_relaxed);
	task->m_unfinished.store(1, std::memory_order_release);
	return task;
}

ipengine::Task * ipengine::ThreadPool::create(TaskFunction && func, TaskContext && context)
{
	//Task * task = new Task();	//allocate from pool later
	Task* task = new(TaskAlloc::allocate(TSIZE))Task;
	task->m_func = std::move(func);
	task->m_context = std::move(context);
	task->m_parent = nullptr;
	task->m_contcount.store(0, std::memory_order_relaxed);
	task->m_refct.store(1, std::memory_order_relaxed);
	task->m_unfinished.store(1, std::memory_order_release);
	return task;
}

void ipengine::ThreadPool::use(Task * task)
{
	task->m_refct.fetch_add(1, std::memory_order_relaxed);
}

void ipengine::ThreadPool::release(Task * task, bool forcerelease)
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

void ipengine::ThreadPool::recycle(Task * task)
{
	task->m_unfinished.store(1, std::memory_order_relaxed);
}

ipengine::ThreadPool::Worker * ipengine::ThreadPool::getWorkerByThreadID(std::thread::id id)
{
	for (size_t i = 0; i < m_workers.size(); i++)
		if (m_workers[i]->m_thread.get_id() == id) return m_workers[i].get();
	return nullptr;
}

// ---------------------------------------- TASK SYSTEM EXTERNAL SECTION ------------------------------------------------------------------

ipengine::ThreadPool::ThreadPool(size_t nworkers) :
	rd(),
	eng(rd()),
	rnd(0, nworkers - 1),
	m_helperqueue()
{
	for (size_t i = 0; i < nworkers; i++)
	{
		m_workers.push_back(alloc_aligned<ThreadPool::Worker, TS_CACHE_LINE_SIZE>(this, i));
	}
	m_isrunning.store(false, std::memory_order_release);
}

ipengine::ThreadPool::~ThreadPool()
{
	for (size_t i = 0; i < m_workers.size(); i++)
	{
		free_aligned(m_workers[i]);
	}
}

void ipengine::ThreadPool::startWorkers()
{
	for (size_t i = 0; i < m_workers.size(); i++)
	{
		m_workers[i]->start();
	}
	m_isrunning.store(true, std::memory_order_release);
}

void ipengine::ThreadPool::stopWorkers()
{
	m_isrunning.store(false, std::memory_order_release);
	for (size_t i = 0; i < m_workers.size(); i++)
	{
		m_workers[i]->stop();
	}
}

bool ipengine::ThreadPool::submit(TaskHandle& handle)
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

bool ipengine::ThreadPool::spawn(TaskHandle& handle, TaskContext* tcptr)
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

ipengine::TaskHandle ipengine::ThreadPool::createTask(const TaskFunction & func, const TaskContext & context)
{
	Task* t = create(func, context);
	return TaskHandle(t, this);
}

ipengine::TaskHandle ipengine::ThreadPool::createTask(const TaskFunction & func, TaskContext && context)
{
	Task* t = create(func, std::move(context));
	return TaskHandle(t, this);
}

ipengine::TaskHandle ipengine::ThreadPool::createTask(TaskFunction && func, const TaskContext & context)
{
	Task* t = create(std::move(func), context);
	return TaskHandle(t, this);
}

ipengine::TaskHandle ipengine::ThreadPool::createTask(TaskFunction && func, TaskContext && context)
{
	Task* t = create(std::move(func), std::move(context));
	return TaskHandle(t, this);
}

ipengine::TaskHandle ipengine::ThreadPool::createEmpty()
{
	Task* t = create(TaskFunction::make_func<empty_func>(), TaskContext());
	return TaskHandle(t, this);
}

ipengine::TaskHandle ipengine::ThreadPool::createChild(const TaskFunction & func, const TaskContext & context, TaskHandle& parent)
{
	if (parent.isValid())
	{
		Task* t = create(func, context);
		if (t == nullptr)
			return TaskHandle();

		//update parent unfinished count via CAS loop. If parents unfinished count becomes 0 we return a invalid task handle
		size_t unf = parent.m_task->m_unfinished.load(std::memory_order_relaxed); //TODO: rethink memory order
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

ipengine::TaskHandle ipengine::ThreadPool::createChild(const TaskFunction & func, TaskContext && context, TaskHandle & parent)
{
	if (parent.isValid())
	{
		Task* t = create(func, std::move(context));
		if (t == nullptr)
			return TaskHandle();

		//update parent unfinished count via CAS loop. If parents unfinished count becomes 0 we return a invalid task handle
		size_t unf = parent.m_task->m_unfinished.load(std::memory_order_relaxed); //TODO: rethink memory order
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

ipengine::TaskHandle ipengine::ThreadPool::createChild(TaskFunction && func, const TaskContext & context, TaskHandle & parent)
{
	if (parent.isValid())
	{
		Task* t = create(std::move(func), context);
		if (t == nullptr)
			return TaskHandle();

		//update parent unfinished count via CAS loop. If parents unfinished count becomes 0 we return a invalid task handle
		size_t unf = parent.m_task->m_unfinished.load(std::memory_order_relaxed); //TODO: rethink memory order
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

ipengine::TaskHandle ipengine::ThreadPool::createChild(TaskFunction && func, TaskContext && context, TaskHandle & parent)
{
	if (parent.isValid())
	{
		Task* t = create(std::move(func), std::move(context));
		if (t == nullptr)
			return TaskHandle();

		//update parent unfinished count via CAS loop. If parents unfinished count becomes 0 we return a invalid task handle
		size_t unf = parent.m_task->m_unfinished.load(std::memory_order_relaxed); //TODO: rethink memory order
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

bool ipengine::ThreadPool::addChild(TaskHandle & parent, TaskHandle & child)
{
	if (parent.isValid() && child.isValid())
	{
		if (parent.m_task == child.m_task->m_parent)
			return true;
		//update parent unfinished count via CAS loop. If parents unfinished count becomes 0 we return false
		size_t unf = parent.m_task->m_unfinished.load(std::memory_order_relaxed); //TODO: rethink memory order
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

bool ipengine::ThreadPool::addContinuation(TaskHandle & task, TaskHandle & continuationTask)
{
	if (task.isValid() && continuationTask.isValid())
	{
		//Check if continuation is already continuation task?
		size_t cindex = task.m_task->m_contcount.load(std::memory_order_relaxed);
		do
		{
			if (cindex >= 15)
			{
				return false;
			}
		} while (!task.m_task->m_contcount.compare_exchange_weak(cindex, cindex + 1, std::memory_order_release, std::memory_order_relaxed));
		use(continuationTask.m_task);
		task.m_task->m_continuations[cindex] = continuationTask.m_task; //synchronozation of this is done with the queues		
	}
	return false;
}

void ipengine::ThreadPool::wait(TaskHandle & handle, TaskContext* tcptr)
{
	if (handle.isValid())
		waitForTask(handle.m_task, tcptr);
}

