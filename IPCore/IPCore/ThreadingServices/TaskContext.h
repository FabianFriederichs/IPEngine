#pragma once
#include <exception>
#include "../Util/any.h"
#include <memory>
#include "../core_config.h"
class CORE_API TaskContext
{
	friend class ThreadPool;
public:
	TaskContext() {}

	template <typename T>
	TaskContext(T data) :
		ex(nullptr),
		data(data),
		workerid(-1)
	{
	}

	TaskContext(const TaskContext& other) :
		ex(std::move(const_cast<TaskContext&>(other).ex)),
		data(other.data),
		workerid(other.workerid)
	{
		
	}

	TaskContext(TaskContext&& other) :
		ex(std::move(other.ex)),
		data(std::move(other.data)),
		workerid(other.workerid)
	{

	}

	TaskContext& operator=(const TaskContext& other)
	{
		if (this == &other)
			return *this;

		TaskContext tmp(other);
		swap(tmp);
		return *this;
	}

	TaskContext& operator=(TaskContext&& other) noexcept
	{
		if (this == &other)
			return *this;
		ex.swap(other.ex);
		data = std::move(other.data);
		workerid = other.workerid;
		return *this;
	}

	void swap(TaskContext& other) noexcept
	{
		/*std::exception e = ex;
		ex = other.ex;
		other.ex = e;*/
		ex.swap(other.ex);
		data.swap(other.data);
		int wid = workerid;
		workerid = other.workerid;
		other.workerid = wid;
	}

	~TaskContext() {}

	template <typename T>
	T& get()
	{
		return data.cast<T>();
	}

	template <typename T>
	const T& get() const
	{
		return data.cast<T>();
	}

	template <typename T>
	operator T&()
	{
		return data.cast<T>();
	}

	template <typename T>
	operator const T&() const
	{
		return data.cast<T>();
	}

	void clear()
	{
		data.clear();
	}

	ThreadPool* getPool()
	{
		return pool;
	}

private:
	//char* ex_msg; //maybe this way. risky when exception is "bad_alloc" but we'd have space for the pool pointer. just new ex_msg on demand and throw a new exception later
	
	soo_any data;
	ThreadPool* pool;
	std::unique_ptr<char[]> ex;
	int workerid; //fixes the "find the worker" thing
};

