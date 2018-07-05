/** \addtogroup typelibrary
Defines common types and data structures for the whole system.
*  @{
*/

/*!
\file LFDS.h
\brief Lock-free data structures

Lock-free data structures are data structures which don't use locks or mutexes to synchronize
access from multiple threads. Instead they're relying on atomic operations implemented in hardware
and memory-order guarantees which those operations give.
They are incredibly hard to implement because its very difficult to reason about the code.
But it's worth it because they have several advantages over the traditional, lock-based datastructures.

They:
\li Decrease contention
\li Decrease latency
\li Increase throughput

This is, because they are essentially the finest-grade locking that one can archieve on the hardware.
Atomic synchronization can be thought of as micro-locks that are acquired for at most the time the
CPU needs to execute the atomic operation.

There are many papers out there which propose new lock-free data structures but many of them
are either very hard to implement or they have drawbacks in terms of portability and performance
in practice.

Over the time some of these data structures shall be implemented in this file.
*/

#ifndef _LOCK_FREE_DATA_STRUCTURES_
#define _LOCK_FREE_DATA_STRUCTURES_
#include <atomic>
#include <IPCore/Util/spinlock.h>
#include <cstdint>
#include <IPCore/ThreadingServices/Task.h>
#define ACQUIRE std::memory_order_acquire
#define RELEASE std::memory_order_release
#define RELAXED std::memory_order_relaxed
#define SEQCST std::memory_order_seq_cst

namespace ipengine {

	/*!
	\brief Implements a growable ring buffer data structure.
	*/
	template <typename T>
	class RingBuffer
	{
	public:
		using Index = uintptr_t;
		using SIndex = intptr_t;

	private:
		SIndex m_logcap;
		T* m_array;

	public:

		RingBuffer();
		/*!
		\brief Initializes the ring buffer with an array of size 2^logcap.
		\param[in] logcap	Initial size of the array is 2^logcap.
		*/
		RingBuffer(SIndex logcap = 10);
		RingBuffer(const RingBuffer<T>& other);
		RingBuffer(RingBuffer<T>&& other);
		~RingBuffer();

		inline T& operator[](SIndex idx);
		inline const T& operator[](SIndex idx) const;
		inline SIndex capacity() const { return static_cast<typename RingBuffer<T>::Index>(1) << static_cast<typename RingBuffer<T>::Index>(m_logcap); }

		RingBuffer<T>* grow(SIndex back, SIndex front);
	};

	template<typename T>
	RingBuffer<T>::RingBuffer() :
		m_logcap(10)
	{
		m_array = new T[static_cast<typename RingBuffer<T>::Index>(1) << static_cast<typename RingBuffer<T>::Index>(10)];
	}

	template<typename T>
	RingBuffer<T>::RingBuffer(typename RingBuffer<T>::SIndex logcap) :
		m_logcap(logcap)
	{
		m_array = new T[static_cast<typename RingBuffer<T>::Index>(1) << static_cast<typename RingBuffer<T>::Index>(logcap)];
	}

	template<typename T>
	RingBuffer<T>::RingBuffer(const RingBuffer<T>& other)
	{
		T* pre = new T[other.capacity()];
		std::memcpy(pre, other.m_array, sizeof(T) * other.capacity());
		m_array = pre;
		m_logcap = other.m_logcap;
	}

	template<typename T>
	RingBuffer<T>::RingBuffer(RingBuffer<T>&& other)
	{
		m_array = other.m_array;
		m_logcap = other.m_logcap;
		other.m_array = nullptr;
		other.m_logcap = 0;
	}

	template<typename T>
	RingBuffer<T>::~RingBuffer()
	{
		if (m_array != nullptr)
		{
			delete[] m_array;
			m_array = nullptr;
		}
	}

	/*!
	\brief Access elements.

	\returns	Returns a reference to the element with index idx mod capacity from the internal array.
	*/
	template<typename T>
	inline T & RingBuffer<T>::operator[](typename RingBuffer<T>::SIndex idx)
	{
		if (m_array != nullptr)
		{
			return m_array[static_cast<typename RingBuffer<T>::Index>(idx) & (static_cast<typename RingBuffer<T>::Index>(capacity()) - static_cast<typename RingBuffer<T>::Index>(1))];
		}
		else
		{
			throw std::out_of_range("Internal array was nullptr.");
		}
	}


	/*!
	\brief Const-access elements.

	\returns	Returns a const reference to the element with index idx mod capacity from the internal array.
	*/
	template<typename T>
	inline const T & RingBuffer<T>::operator[](typename RingBuffer<T>::SIndex idx) const
	{
		if (m_array != nullptr)
		{
			return m_array[static_cast<typename RingBuffer<T>::Index>(idx) & (static_cast<typename RingBuffer<T>::Index>(capacity()) - static_cast<typename RingBuffer<T>::Index>(1))];
		}
		else
		{
			throw std::out_of_range("Internal array was nullptr.");
		}
	}
	
	/*!
	\brief Returns a new ring buffer with double the size of the old one and all elements copied over.
	*/
	template<typename T>
	RingBuffer<T>* RingBuffer<T>::grow(typename RingBuffer<T>::SIndex back, typename RingBuffer<T>::SIndex front)
	{
		RingBuffer<T>* newarr = new RingBuffer<T>(m_logcap + 1);
		for (SIndex i = front; i < back; i++)
		{
			(*newarr)[i] = (*this)[i];
		}
		return newarr;
	}

	//-------------------------------- LOCK FREE STACK ---------------------------------------------------------------------------------------------------

	/*!
	\brief Not implemented yet.
	*/
	template <typename T>
	class LockFreeStack
	{
	public:
		LockFreeStack();
		LockFreeStack(size_t _lcap);
		LockFreeStack(const LockFreeStack& other);
		LockFreeStack(LockFreeStack&& other);

		bool push(const T& item);
		bool push(T&& item);

		bool pop(T& item);

		size_t size();
		size_t capacity();


	private:

	private:
		T* m_arr;
		size_t m_logcap;
		std::atomic<size_t> m_head;
		std::atomic<size_t> m_commitpos;
	};

	template<typename T>
	inline size_t LockFreeStack<T>::capacity()
	{
		return size_t(1) << m_logcap;
	}

	template<typename T>
	inline LockFreeStack<T>::LockFreeStack() :
		m_logcap(10),
		m_arr(new T[size_t(1) << 10])
	{
	}

	template<typename T>
	inline LockFreeStack<T>::LockFreeStack(size_t _lcap) :
		m_logcap(_lcap),
		m_arr(new T[size_t(1) << _lcap])
	{
	}

	template<typename T>
	inline LockFreeStack<T>::LockFreeStack(const LockFreeStack & other)
	{
	}

	template<typename T>
	inline LockFreeStack<T>::LockFreeStack(LockFreeStack && other)
	{
	}

	template<typename T>
	inline bool LockFreeStack<T>::push(const T & item)
	{
		size_t curhead = m_head.load();
		if (curhead >= capacity())
			return false;	//later: let the interal array grow

		while (!m_head.compare_exchange_weak(curhead, curhead + 1));

		m_arr[curhead] = item;

		m_commitpos.store(curhead);
		return true;
	}

	template<typename T>
	inline bool LockFreeStack<T>::push(T && item)
	{
		size_t curhead = m_head.load();
		if (curhead >= capacity())
			return false;	//later: let the interal array grow

		while (!m_head.compare_exchange_weak(curhead, curhead + 1));

		m_arr[curhead] = std::move(item);

		m_commitpos.store(curhead);
		return true;
	}

	template<typename T>
	inline bool LockFreeStack<T>::pop(T & item)
	{
		return false;
	}

	template<typename T>
	inline size_t LockFreeStack<T>::size()
	{
		return m_head.load() + 1;
	}

	//------------------------------- LOCK FREE QUEUE ---------------------------------------------------------------------------------------------------

	/*!
	\brief Not implemented yet.
	*/
	template <typename T>
	class LockFreeQueue
	{

	};

	//----------------- http://www.di.ens.fr/~zappa/readings/ppopp13.pdf ---------------- A LOCK FREE WORK STEALING DEQUE FOR THE WORKERS -----------------------------------------

	/*!
	\brief Implementation of the Chase-Lev work stealing queue.
	
	"Dynamic Circular Work-Stealing Deque"; David Chase, Yossi Lev; 2005
	http://www.di.ens.fr/~zappa/readings/ppopp13.pdf

	The steal operation is thread safe, push and pop are not.
	This data structure is used for the local work queues of the workers in the thread pool system.

	If the RingBuffer has to grow, the old RingBuffer instance is enqueued for deallocation
	when no thread accesses it anymore.
	*/
	template <typename T>
	class LockFreeWSQueue
	{
	private:
		//data structures
		typedef struct del_node
		{
			RingBuffer<T>* data;
			del_node* next;
		} del_node;



		std::atomic<RingBuffer<T>*> m_items;
		std::atomic<typename RingBuffer<T>::SIndex> m_front;
		std::atomic<typename RingBuffer<T>::SIndex> m_back;
		//hp_manager<100> m_hpman;
		del_node* m_arrays_to_del;
		std::atomic<unsigned int> m_threads_in_steal;


		void try_delete_arr(RingBuffer<T>* oldarr);
		void try_delete_arr();

	public:
		LockFreeWSQueue()
		{
			m_back.store(0);
			m_front.store(0);
			m_items.store(new RingBuffer<T>(10));
			m_arrays_to_del = nullptr;
			m_threads_in_steal.store(0);
		}

		LockFreeWSQueue(typename RingBuffer<T>::SIndex caplog)
		{
			m_back.store(0);
			m_front.store(0);
			m_items.store(new RingBuffer<T>(caplog));
			m_threads_in_steal.store(0);
		}

		LockFreeWSQueue(const LockFreeWSQueue& other)
		{
			m_back.store(other.m_back.load());
			m_front.store(other.m_front.load());
			m_items.store(new RingBuffer<T>(*other.m_items.load()));
			m_threads_in_steal.store(other.m_threads_in_steal.load());
		}

		LockFreeWSQueue(LockFreeWSQueue&& other)
		{
			m_back.store(other.m_back.load());
			m_front.store(other.m_front.load());
			m_items.store(other.m_items.load());
			other.m_items.store(nullptr);
			m_threads_in_steal.store(other.m_threads_in_steal.load());
		}

		~LockFreeWSQueue()
		{
			auto arr = m_items.load();
			if (arr != nullptr)
				delete arr;
		}

		//public interface
		bool push(const T& item);
		bool push(T&& item);
		//bool try_pop(T& target);		
		bool pop(T& target);
		bool steal(T& target);
		//bool try_steal(T& target);
		typename RingBuffer<T>::SIndex size();			//return current size
		typename RingBuffer<T>::SIndex capacity();	//current capacity
		bool empty();		//true if empty
		void clear();		//remove all m_items
	};

	//TODO: refine memory orderings when this stuff works

	//! Try to delete the old RingBuffer instance or enqueue it for later deallocation
	template<typename T>
	inline void LockFreeWSQueue<T>::try_delete_arr(RingBuffer<T>* oldarr)
	{
		if (m_threads_in_steal.load() == 0)
		{
			std::cout << "Deleting last array\n";
			delete oldarr;
		}
		else
		{
			//push oldarr onto delete list
			del_node* newnode = new del_node{ oldarr, m_arrays_to_del };
			m_arrays_to_del = newnode;
			std::cout << "Pushed old array to del list\n";
		}
		if (m_arrays_to_del != nullptr)
			try_delete_arr();
	}

	//! Iterate through the to-be-deleted list and try to delete some of the old RingBuffer instances.
	template<typename T>
	inline void LockFreeWSQueue<T>::try_delete_arr() //one problem here: although it's very likely that a dead array is eventually deleted, it could theoretically happen that the array stays in memory forever if contention on steal is too high
	{
		del_node* rp = m_arrays_to_del;
		m_arrays_to_del = nullptr;
		while (rp)
		{
			auto next = rp->next;
			if (m_threads_in_steal.load() == 0)
			{
				delete rp->data;
				delete rp;
			}
			else
			{
				rp->next = m_arrays_to_del;
				m_arrays_to_del = rp;
			}
			rp = next;
		}
	}

	//! Push an item onto the private end of the queue. Not thread safe.
	template <typename T>
	bool LockFreeWSQueue<T>::push(const T& item)
	{
		typename RingBuffer<T>::SIndex back = m_back.load(RELAXED);
		typename RingBuffer<T>::SIndex front = m_front.load(RELAXED);
		if (back - front >= m_items.load(RELAXED)->capacity() - 1)
		{
			auto oldarr = m_items.load(RELAXED);
			auto newarr = oldarr->grow(back, front);
			if (newarr == nullptr)
				return false;
			m_items.store(newarr, RELEASE);
			try_delete_arr(oldarr);
		}
		(*m_items.load(RELAXED))[back] = item;
		//rfence
		std::atomic_thread_fence(RELEASE);
		m_back.store(back + 1, RELAXED);

		if (m_arrays_to_del != nullptr)
			try_delete_arr();

		return true;
	}

	//! Push an item onto the private end of the queue. Not thread safe.
	template <typename T>
	bool LockFreeWSQueue<T>::push(T&& item)
	{
		typename RingBuffer<T>::SIndex back = m_back.load(RELAXED);
		typename RingBuffer<T>::SIndex front = m_front.load(RELAXED);
		if (back - front >= m_items.load(RELAXED)->capacity() - 1)
		{
			auto oldarr = m_items.load(RELAXED);
			auto newarr = oldarr->grow(back, front);
			if (newarr == nullptr)
				return false;
			m_items.store(newarr, RELEASE);
			try_delete_arr(oldarr);
		}
		(*m_items.load(RELAXED))[back] = std::move(item);
		//rfence
		std::atomic_thread_fence(RELEASE);
		m_back.store(back + 1, RELAXED);

		if (m_arrays_to_del != nullptr)
			try_delete_arr();

		return true;
	}

	/*! 
	\brief Pop an item from the private end of the queue. Not thread safe.
	
	Copies the item into target.

	\param[out] target  Instance the item should be copied to.
	\returns			Returns true if an item was popped. Returns false if the queue was empty.
	*/
	template <typename T>
	bool LockFreeWSQueue<T>::pop(T& target)
	{
		T tmp;
		typename RingBuffer<T>::SIndex back = m_back.load(RELAXED) - 1;
		m_back.store(back, RELAXED);
		//fence
		std::atomic_thread_fence(SEQCST);
		typename RingBuffer<T>::SIndex front = m_front.load(RELAXED);
		if (front <= back)
		{
			tmp = (*m_items.load(RELAXED))[back];
			if (front != back)
			{
				target = std::move(tmp);
				return true;
			}

			typename RingBuffer<T>::SIndex tmpfront = front;
			bool win = m_front.compare_exchange_strong(tmpfront, tmpfront + 1, SEQCST, RELAXED);

			m_back.store(front + 1, RELAXED);

			if (win)
			{
				target = std::move(tmp);
				return true;
			}
			return false;
		}
		else
		{
			m_back.store(front, RELAXED);
			return false;
		}
	}

	/*!
	\brief Steals an item from the public end of the queue. Thread safe.

	Copies the item into target.

	\param[out] target  Instance the item should be copied to.
	\returns			Returns true if an item was stolen. Returns false if the queue was empty.
	*/
	template <typename T>
	bool LockFreeWSQueue<T>::steal(T& target)
	{
		T tmp;
		typename RingBuffer<T>::SIndex front = m_front.load(ACQUIRE);
		//fence
		std::atomic_thread_fence(SEQCST);
		typename RingBuffer<T>::SIndex back = m_back.load(ACQUIRE);

		if (front < back)
		{
			m_threads_in_steal.fetch_add(1);
			tmp = (*m_items.load(RELAXED))[front];
			m_threads_in_steal.fetch_sub(1);

			if (!m_front.compare_exchange_strong(front, front + 1, SEQCST, RELAXED))
			{
				return false;
			}

			target = std::move(tmp);
			return true;
		}
		else
		{
			return false;
		}
	}

	/*!
	\brief Returns the number of items currently in the queue.
	*/
	template <typename T>
	typename RingBuffer<T>::SIndex LockFreeWSQueue<T>::size()
	{
		typename RingBuffer<T>::SIndex s = m_back.load() - m_front.load();
		if (s < 0)
			return 0;
		return s;
	}

	/*!
	\brief Returns the current capacity of the underlying ring buffer.
	*/
	template <typename T>
	typename RingBuffer<T>::SIndex LockFreeWSQueue<T>::capacity()
	{
		return m_items.load()->capacity();
	}

	/*!
	\brief Returns true if the queue is empty.
	*/
	template <typename T>
	bool LockFreeWSQueue<T>::empty()
	{
		return m_front.load() >= m_back.load();
	}

	//! Not implemented yet.
	template <typename T>
	void LockFreeWSQueue<T>::clear()
	{

	}


}
#endif
/** @}*/