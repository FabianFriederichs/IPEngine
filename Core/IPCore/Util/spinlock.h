/** \addtogroup threading
*  @{
*/

/*!
\file spinlock.h
\brief Some spinlock implementations.

Spinlocks are very useful in situations where little contention is expected on a lock.
Spinlock often do not need system calls in such situation, instead they busy-wait
for a short time. The basic spinlock simply busy-waits until the lock can be acquired,
other variants implement some strategy to reduce wasted cpu time if they have to wait
longer than a specific period of time.

All the spinlock classes are compatible with std::lock_guard and std::unique_lock.

The locks imply acquire semantics for the lock() functions and release semantics for the release() functions,
regarding memory order.

\todo Implement support for recursive locking
*/

#ifndef _SPIN_LOCK_H_
#define _SPIN_LOCK_H_
#include <atomic>
#include <chrono>
#include <random>
#include <algorithm>
#include <IPCore/core_config.h>

namespace ipengine {

	/*!
	\brief Basic spinlock implementation.

	Busy-waits until the lock is acquired.
	*/
	class alignas(TS_CACHE_LINE_SIZE)BasicSpinLock
	{
	private:
		alignas(TS_CACHE_LINE_SIZE)std::atomic_flag lck = ATOMIC_FLAG_INIT;

	public:
		//ctors/dtors
		BasicSpinLock()
		{
		}
		~BasicSpinLock()
		{

		}
		BasicSpinLock(const BasicSpinLock& other) = delete;
		BasicSpinLock(BasicSpinLock&& other) = delete;

		//! Busy-waits until the lock is acquired.
		inline void lock()
		{
			while (lck.test_and_set(std::memory_order_acquire));
		}

		//! Releases the lock.
		inline void unlock() noexcept
		{
			lck.clear(std::memory_order_release);
		}

		/*!
		\brief Tries to acquire the lock.
		\returns	Returns true if the lock was acquired successfully, false otherwise.
		*/
		inline bool try_lock() noexcept
		{
			return lck.test_and_set(std::memory_order_acquire);
		}
	};

	/*!
	\brief Yields after every iteration and sleeps for a short period of time after MAX_SPINS iterations.

	\tparam MAX_SPINS	Thread is put to sleep after MAX_SPINS iterations of waiting.
	*/
	template <size_t MAX_SPINS = 4000>
	class alignas(TS_CACHE_LINE_SIZE)YieldingSpinLock
	{
	private:
		alignas(TS_CACHE_LINE_SIZE)std::atomic_flag lck = ATOMIC_FLAG_INIT;

	public:
		//ctors/dtors
		YieldingSpinLock()
		{
		}

		~YieldingSpinLock()
		{

		}
		YieldingSpinLock(const YieldingSpinLock& other) = delete;
		YieldingSpinLock(YieldingSpinLock&& other) = delete;

		/*!
		\brief Acquires the lock.

		Yields after each iteration and sleeps for 500us when the number of iterations reached MAX_SPINS.
		*/
		inline void lock()
		{
			size_t iterations = 0;
			while (lck.test_and_set(std::memory_order_acquire))
			{
				//std::cout << "LF";
				if (iterations > MAX_SPINS)
				{
					//std::cout << "SF";
					std::this_thread::sleep_for(std::chrono::microseconds(500));
				}
				++iterations;
				std::this_thread::yield();
			}
		}

		//! Releases the lock.
		inline void unlock() noexcept
		{
			lck.clear(std::memory_order_release);
		}

		/*!
		\brief Tries to acquire the lock.

		\returns	Returns true of the lock was acquired, false otherwise.
		*/
		inline bool try_lock() noexcept
		{
			return lck.test_and_set(std::memory_order_acquire);
		}
	};

	/*!
	\brief Same as the other two implementations but with randomized, exponential backoff.

	This implementation was not very useful in the tests, rather use the standard std::mutex and
	see if it delivers better performance, first.
	*/
	template <size_t MAX_SPINS = 4000, size_t MAX_BACKOFF = 4000>
	class alignas(TS_CACHE_LINE_SIZE)TTASSpinLock
	{
	private:
		alignas(TS_CACHE_LINE_SIZE)std::atomic<bool> lck;

	public:
		//ctors/dtors
		TTASSpinLock()
		{
			lck.store(false);
		}

		~TTASSpinLock()
		{

		}
		TTASSpinLock(const TTASSpinLock& other) = delete;
		TTASSpinLock(TTASSpinLock&& other) = delete;


		inline void lock()
		{
			size_t iterations = 0;
			while (true)
			{
				waituntillf();

				if (lck.exchange(true, std::memory_order_acquire))
					expBackoff(iterations);
				else
					break;
			}
		}

		void waituntillf()
		{
			size_t iterations = 0;
			while (lck.load(std::memory_order_relaxed))
			{
				if (iterations < MAX_SPINS)
				{
					std::this_thread::yield();
					iterations++;
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::microseconds(500));
				}
			}
		}

		static void expBackoff(size_t& iters)
		{
			thread_local std::uniform_int_distribution<size_t> dist;
			thread_local std::minstd_rand gen(std::random_device{}());
			const size_t spinIters = dist(gen, decltype(dist)::param_type{ 0, iters });

			iters = std::min(2 * iters, MAX_BACKOFF);
			for (volatile size_t i = 0; i < spinIters; i++) std::this_thread::yield(); // Avoid being optimized out!
		}

		inline void unlock() noexcept
		{
			lck.store(false, std::memory_order_release);
		}

		inline bool try_lock() noexcept
		{
			return lck.exchange(true, std::memory_order_acquire);
		}
	};

}
#endif
/** @}*/