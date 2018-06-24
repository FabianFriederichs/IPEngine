/** \addtogroup threading
*  @{
*/

/*!
\file TaskContext.h
*/

#ifndef _TASK_CONTEXT_H_
#define _TASK_CONTEXT_H_
#include <exception>
#include <IPCore/Util/any.h>
#include <memory>
#include <IPCore/core_config.h>

#include <IPCore/ThreadingServices/Common.h>

namespace ipengine
{
	class ThreadPool;

	/*!
	\brief Holds the execution context for a single task.

	The class holds arbitrary data, type erased via soo_any, an error message and a worker token which is used as a
	hint by the thread pool and slightly speeds up some operations.
	*/
	class CORE_API TaskContext
	{
		friend class ThreadPool;
	public:
		//! Initializes an empty task context.
		TaskContext() :
			ex(nullptr),
			data(),
			wtok()
		{}

		/*!
		\brief Initializes the task context with some arbitrary data.

		The soo_any member is initilaized with data.
		soo_any provides small object optimization up to 16 bytes, so it's best to
		pass pointers to structs as it prevents heap allocations.

		\tparam T	Type of data.
		\param[in] data Data to initialize the task context with.
		*/
		template <typename T>
		TaskContext(T data) :
			ex(nullptr),
			data(data),
			wtok()
		{
		}

		//! Copy constructor
		TaskContext(const TaskContext& other) :
			ex(std::move(const_cast<TaskContext&>(other).ex)),
			data(other.data),
			wtok(other.wtok)
		{

		}

		//! Move constructor
		TaskContext(TaskContext&& other) :
			ex(std::move(other.ex)),
			data(std::move(other.data)),
			wtok(other.wtok)
		{

		}

		//! Copy assignment
		TaskContext& operator=(const TaskContext& other)
		{
			if (this == &other)
				return *this;

			TaskContext tmp(other);
			swap(tmp);
			return *this;
		}

		//! Move assignment
		TaskContext& operator=(TaskContext&& other) noexcept
		{
			if (this == &other)
				return *this;
			ex.swap(other.ex);
			data = std::move(other.data);
			wtok = other.wtok;
			return *this;
		}

		//! Swaps internal state of two instances
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

		//! Destructor
		~TaskContext() {}

		/*!
		\brief Tries to get the data stored as a reference to T.

		\tparam T	Type the data should be cast into.
		\throws		Throws std::bad_cast if the stored data can't be converted to T.
		*/
		template <typename T>
		T& get()
		{
			return data.cast<T>();
		}

		/*!
		\brief Tries to get the data stored as a const reference to T.

		\tparam T	Type the data should be cast into.
		\throws		Throws std::bad_cast if the stored data can't be converted to T.
		*/
		template <typename T>
		const T& get() const
		{
			return data.cast<T>();
		}

		/*!
		\brief Implicit cast into T&.

		\tparam T	Type the data should be cast into.
		\throws		Throws std::bad_cast if the stored data can't be converted to T.
		*/
		template <typename T>
		operator T&()
		{
			return data.cast<T>();
		}

		/*!
		\brief Implicit cast into const T&.

		\tparam T	Type the data should be cast into.
		\throws		Throws std::bad_cast if the stored data can't be converted to T.
		*/
		template <typename T>
		operator const T&() const
		{
			return data.cast<T>();
		}

		/*!
		\implicit Clears the stored data.
		*/
		void clear()
		{
			data.clear();
		}

		/*!
		\brief Returns a pointer to the thread pool the task was submitted to.
		*/
		ThreadPool* getPool()
		{
			return pool;
		}

		/*!
		\brieg Returns the worker token.
		*/
		ipengine::WorkerToken getWorkerToken()
		{
			return wtok;
		}

	private:
		//char* ex_msg; //maybe this way. risky when exception is "bad_alloc" but we'd have space for the pool pointer. just new ex_msg on demand and throw a new exception later
		//max 72
		//! Arbitrary data used as parameter in the task function
		ipengine::soo_any data;								//48
		//! ThreadPool instance this context was created by
		ThreadPool* pool;									//8
		//! If an exception occures during execution of a task, the message is stored here.
		std::unique_ptr<char[]> ex;							//8
		//! Worker token. Indicates which worker the task is executed on.
		WorkerToken wtok;//int workerid; //fixes the "find the worker" thing	//4
	};

}
#endif

/** @}*/