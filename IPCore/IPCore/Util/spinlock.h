#pragma once
#include <atomic>
#include <chrono>
#include <random>
#include <algorithm>
#include <core_config.h>

namespace ipengine {

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


		inline void lock()
		{
			while (lck.test_and_set(std::memory_order_acquire));
		}

		inline void unlock() noexcept
		{
			lck.clear(std::memory_order_release);
		}

		inline bool try_lock() noexcept
		{
			return lck.test_and_set(std::memory_order_acquire);
		}
	};

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

		inline void unlock() noexcept
		{
			lck.clear(std::memory_order_release);
		}

		inline bool try_lock() noexcept
		{
			return lck.test_and_set(std::memory_order_acquire);
		}
	};

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