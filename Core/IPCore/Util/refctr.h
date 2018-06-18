/** \addtogroup util
*  @{
*/

/*!
\file refctr.h
*/
#ifndef _REF_CT_H_
#define _REF_CT_H_
#include <atomic>

namespace ipengine {

	/*!
	\brief A thread safe, lock-free implementation of a reference counter.
	*/
	class RefCtr
	{
		std::atomic<size_t> rc;
	public:
		//! Initializes the reference count with 0.
		RefCtr()
		{
			rc.store(0, std::memory_order_relaxed);
		}

		//! Copy contructor.
		RefCtr(const RefCtr& other) :
			rc(other.rc.load(std::memory_order_relaxed))
		{

		}

		//! Move contructor.
		RefCtr(RefCtr&& other) :
			rc(other.rc.load(std::memory_order_relaxed))
		{

		}

		/*!
		\brief Initializes the reference count with init.
		\param[in] init		The value, the reference count is initialized with.
		*/
		RefCtr(size_t init)
		{
			rc.store(init, std::memory_order_relaxed);
		}

		//! Increments the reference count.
		inline void inc()
		{
			rc.fetch_add(1, std::memory_order_relaxed);
		}

		/*!
		\brief Decrements the reference count and returns true if it reached 0.

		\returns	Returns true if the renference count has reached 0.
		*/
		inline bool dec()
		{
			if (rc.fetch_sub(1, std::memory_order_release) == 1)
			{
				std::atomic_thread_fence(std::memory_order_acquire);
				return true;
			}
			return false;
		}
	};
}

#endif
/** @}*/