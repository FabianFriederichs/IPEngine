#ifndef _REF_CT_H_
#define _REF_CT_H_
#include <atomic>

namespace ipengine {

	class RefCtr
	{
		std::atomic<size_t> rc;
	public:
		RefCtr()
		{
			rc.store(0, std::memory_order_relaxed);
		}

		RefCtr(const RefCtr& other) :
			rc(other.rc.load(std::memory_order_relaxed))
		{

		}

		RefCtr(RefCtr&& other) :
			rc(other.rc.load(std::memory_order_relaxed))
		{

		}

		RefCtr(size_t init)
		{
			rc.store(init, std::memory_order_relaxed);
		}

		inline void inc()
		{
			rc.fetch_add(1, std::memory_order_relaxed);
		}

		//returns true if reference count reaches zero
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