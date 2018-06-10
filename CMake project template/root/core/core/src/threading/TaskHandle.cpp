#include <IPCore/ThreadingServices/TaskHandle.h>
#include <IPCore/ThreadingServices/ThreadPool.h>

ipengine::TaskHandle::TaskHandle() :
	m_task(nullptr),
	m_pool(nullptr)
{
}

ipengine::TaskHandle::TaskHandle(Task * task, ThreadPool* pool) :
	m_task(task),
	m_pool(pool),
	m_isinpool(false)
{
}

ipengine::TaskHandle::TaskHandle(const TaskHandle & _other)
{
	if (this == &_other)
		return;
	if (_other.isValid())
	{
		_other.m_pool->use(_other.m_task);
		m_task = _other.m_task;
		m_pool = _other.m_pool;
		m_isinpool = _other.m_isinpool;
	}
	else
	{
		m_task = nullptr;
		m_pool = nullptr;
		m_isinpool = false;
	}
}

ipengine::TaskHandle::TaskHandle(TaskHandle && _other)
{
	if (this == &_other)
		return;
	if (_other.isValid())
	{
		m_task = _other.m_task;
		m_pool = _other.m_pool;
		m_isinpool = _other.m_isinpool;
		_other.m_pool = nullptr;
		_other.m_task = nullptr;
		_other.m_isinpool = false;
	}
	else
	{
		m_task = nullptr;
		m_pool = nullptr;
		m_isinpool = false;
	}
}

ipengine::TaskHandle::~TaskHandle()
{
	if (isValid())
	{
		m_pool->release(m_task);
		m_task = nullptr;
	}
}


//we'll need a reference to a threadpool. cyclic dependency. outch
bool ipengine::TaskHandle::wait(WorkerToken wtok)
{
	if (isValid() && m_isinpool)
	{
		m_pool->wait(*this, wtok);
		m_isinpool = false;
		return true;
	}
	return false;
}

bool ipengine::TaskHandle::wait_recycle(WorkerToken wtok)
{
	if (isValid() && m_isinpool)
	{
		m_pool->wait(*this, wtok);
		m_isinpool = false;
		m_pool->recycle(m_task);	//reset unifnished count to 1. Now the task can be thrown into the pool again.
		return true;
	}
	return false;
}

bool ipengine::TaskHandle::addChild(TaskHandle & child)
{
	if (isValid())
	{
		return m_pool->addChild(*this, child);
	}
	return false;
}

bool ipengine::TaskHandle::addContinuation(TaskHandle & continuationTask)
{
	if (isValid())
	{
		return m_pool->addContinuation(*this, continuationTask);
	}
	return false;
}

bool ipengine::TaskHandle::spawn(WorkerToken wtok)
{
	if (isValid() && !m_isinpool)
	{
		if (m_pool->spawn(*this, wtok))
		{
			m_isinpool = true;
			return true;
		}
	}
	m_isinpool = false;
	return false;
}

bool ipengine::TaskHandle::submit()
{
	if (isValid() && !m_isinpool)
	{
		if (m_pool->submit(*this))
		{
			m_isinpool = true;
			return true;
		}
	}
	m_isinpool = false;
	return false;
}

void ipengine::TaskHandle::execute()
{
	if (isValid() && !m_isinpool)
	{
		m_pool->executeImmediate(*this);
	}
}

ipengine::TaskContext * ipengine::TaskHandle::getContext()
{	
	if (isValid())
		return &m_task->m_context;
	else
		return nullptr;	
}
