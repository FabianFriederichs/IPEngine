#pragma once
#include <exception>
#include <IPCore/Util/any.h>
#include <memory>
#include <IPCore/core_config.h>

#include <IPCore/ThreadingServices/Common.h>

namespace ipengine
{
	class ThreadPool;

	class CORE_API TaskContext
	{
		friend class ThreadPool;
	public:
		TaskContext() :
			ex(nullptr),
			data(),
			wtok()
		{}

		template <typename T>
		TaskContext(T data) :
			ex(nullptr),
			data(data),
			wtok()
		{
		}

		//TODO: fix this constructor
		TaskContext(const TaskContext& other) :
			ex(std::move(const_cast<TaskContext&>(other).ex)),
			data(other.data),
			wtok(other.wtok)
		{

		}

		TaskContext(TaskContext&& other) :
			ex(std::move(other.ex)),
			data(std::move(other.data)),
			wtok(other.wtok)
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
			wtok = other.wtok;
			return *this;
		}

		void swap(TaskContext& other) noexcept
		{
			/*std::exception e = ex;
			ex = other.ex;
			other.ex = e;*/
			ex.swap(other.ex);
			data.swap(other.data);
			WorkerToken w = wtok;
			wtok = other.wtok;
			other.wtok = w;
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

		ipengine::WorkerToken getWorkerToken()
		{
			return wtok;
		}

	private:
		//char* ex_msg; //maybe this way. risky when exception is "bad_alloc" but we'd have space for the pool pointer. just new ex_msg on demand and throw a new exception later
		//max 72
		ipengine::soo_any data;								//48
		ThreadPool* pool;									//8
		std::unique_ptr<char[]> ex;							//8
		WorkerToken wtok;//int workerid; //fixes the "find the worker" thing	//4
	};

}

