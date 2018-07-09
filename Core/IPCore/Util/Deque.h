/** \addtogroup typelibrary
*  @{
*/

/*!
\file Dequeue.h
*/
#ifndef _DE_QUEUE_H_
#define _DE_QUEUE_H_
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <IPCore/Util/spinlock.h>
#include <IPCore/Memory/lowlevel_allocators.h>

namespace ipengine {

	template <typename T>
	class DQNode
	{
	public:
		//delete if possible
		DQNode() : m_item() : m_item(), m_next(nullptr), m_prev(nullptr)
		{
		}

		DQNode(const T& item) : m_item(item), m_next(nullptr), m_prev(nullptr)
		{
		}

		DQNode(T&& item) : m_item(std::move(item)), m_next(nullptr), m_prev(nullptr)
		{
		}

		DQNode(const T& item, DQNode<T>* next, DQNode<T>* prev) : m_item(item), m_next(next), m_prev(prev)
		{
		}

		DQNode(T&& item, DQNode<T>* next, DQNode<T>* prev) : m_item(std::move(item)), m_next(next), m_prev(prev)
		{
		}

		~DQNode()
		{
		}

		T m_item;
		DQNode<T>* m_next;
		DQNode<T>* m_prev;
	};

	//make this stuff lock free in the future. for testing now, i'll stick to a lock based implementation.
	/*!
	\brief A basic, node-based implementation of a thread safe double-ended queue.

	Nodes are allocated using the FreeList implementation to prevent memory fragmentation.
	*/
	template <typename T>
	class Deque
	{
		const size_t nsz = sizeof(DQNode<T>);
	public:
		Deque();
		~Deque();

		//! Pushes an item right
		bool push_right(const T& item);
		//! Pushes an item left
		bool push_left(const T& item);
		//! Pushes an item right
		bool push_right(T&& item);
		//! Pushes an item left
		bool push_left(T&& item);
		//! Pops an item left, thread waits if empty
		bool pop_left(T& target);			//pop, wait if empty
		//! Tries to pop an item left, returns false if empty.
		bool try_pop_left(T& target);		//pop, return empty shared_ptr if empty
		//! Pops an item right, thread waits if empty
		bool pop_right(T& target);			//steal, wait if empty
		//! Tries to pop an item right, returns false if empty.
		bool try_pop_right(T& target);		//steal, return empty shared_ptr if empty
		//! Returns the current number of items in the queue
		size_t size();			//return current size
		//! Returns true if the queue is empty
		bool empty();		//true if empty
		//! Empties the whole queue
		void clear();		//remove all items

	private:
		DQNode<T>* m_left;

		DQNode<T>* m_right;

		std::atomic<size_t> m_size;

		//Thread safe later
		using mutex_t = YieldingSpinLock<5000>;
		mutex_t m_item_mtx;

		std::condition_variable m_pop_cond;

		FreeList<128, sizeof(DQNode<T>), 4096> NodeAlloc;

	};



	//------------------------ *tors -----------------------------------------------

	template <typename T>
	Deque<T>::Deque() : m_size(0), m_right(nullptr), m_left(nullptr)	//throws if allocation explodes. should be safe
	{
		if (!NodeAlloc.initialize())
			throw std::bad_alloc();
	}

	template <typename T>
	Deque<T>::~Deque()
	{
		clear();
	}

	//------------------------ public interface -------------------------------------

	template <typename T>
	bool Deque<T>::push_right(const T& item)
	{
		std::unique_lock<mutex_t> lock(m_item_mtx);
		DQNode<T>* right_node = m_right;
		if (right_node == nullptr)	//first node to be inserted
		{
			try
			{
				//DQNode<T>* new_node = new DQNode<T>(item);
				DQNode<T>* new_node = new (NodeAlloc.allocate(nsz))DQNode<T>(item);
				m_right = new_node;
				m_left = new_node;
				m_size.fetch_add(1, std::memory_order_relaxed);
			}
			catch (const std::bad_alloc&)
			{
				return false;
			}

		}
		else
		{
			try
			{
				//DQNode<T>* new_node = new DQNode<T>(item);
				DQNode<T>* new_node = new (NodeAlloc.allocate(nsz))DQNode<T>(item);
				new_node->m_prev = right_node;
				right_node->m_next = new_node;
				m_right = new_node;
				m_size.fetch_add(1, std::memory_order_relaxed);
			}
			catch (const std::bad_alloc&)
			{
				return false;
			}
		}
		lock.unlock();
		m_pop_cond.notify_one();
		return true;
	}

	template <typename T>
	bool Deque<T>::push_right(T&& item)
	{
		std::unique_lock<mutex_t> lock(m_item_mtx);
		DQNode<T>* right_node = m_right;
		if (right_node == nullptr)	//first node to be inserted
		{
			try
			{
				//DQNode<T>* new_node = new DQNode<T>(std::move(item));
				DQNode<T>* new_node = new (NodeAlloc.allocate(nsz))DQNode<T>(std::move(item));
				m_right = new_node;
				m_left = new_node;
				m_size.fetch_add(1, std::memory_order_relaxed);
			}
			catch (const std::bad_alloc& ex)
			{
				return false;
			}
		}
		else
		{
			try
			{
				//DQNode<T>* new_node = new DQNode<T>(std::move(item));
				DQNode<T>* new_node = new (NodeAlloc.allocate(nsz))DQNode<T>(std::move(item));
				new_node->m_prev = right_node;
				right_node->m_next = new_node;
				m_right = new_node;
				m_size.fetch_add(1, std::memory_order_relaxed);
			}
			catch (const std::bad_alloc& ex)
			{
				return false;
			}
		}
		lock.unlock();
		m_pop_cond.notify_one();
		return true;
	}

	template <typename T>
	bool Deque<T>::push_left(const T& item)
	{
		std::unique_lock<mutex_t> lock(m_item_mtx);
		DQNode<T>* left_node = m_left;
		if (left_node == nullptr)
		{
			try
			{
				//DQNode<T>* new_node = new DQNode<T>(item);
				DQNode<T>* new_node = new (NodeAlloc.allocate(nsz))DQNode<T>(item);
				m_right = new_node;
				m_left = new_node;
				m_size.fetch_add(1, std::memory_order_relaxed);
			}
			catch (const std::bad_alloc&)
			{
				return false;
			}
		}
		else
		{
			try
			{
				//DQNode<T>* new_node = new DQNode<T>(item);
				DQNode<T>* new_node = new (NodeAlloc.allocate(nsz))DQNode<T>(item);
				left_node->m_prev = new_node;
				new_node->m_next = left_node;
				m_left = new_node;
				m_size.fetch_add(1, std::memory_order_relaxed);
			}
			catch (const std::bad_alloc&)
			{
				return false;
			}
		}
		lock.unlock();
		m_pop_cond.notify_one();
		return true;
	}

	template <typename T>
	bool Deque<T>::push_left(T&& item)
	{
		std::unique_lock<mutex_t> lock(m_item_mtx);
		DQNode<T>* left_node = m_left;
		if (left_node == nullptr)
		{
			try
			{
				//DQNode<T>* new_node = new DQNode<T>(std::move(item));
				DQNode<T>* new_node = new (NodeAlloc.allocate(nsz))DQNode<T>(std::move(item));
				m_right = new_node;
				m_left = new_node;
				m_size.fetch_add(1, std::memory_order_relaxed);
			}
			catch (const std::bad_alloc& ex)
			{
				return false;
			}
		}
		else
		{
			try
			{
				//DQNode<T>* new_node = new DQNode<T>(std::move(item));
				DQNode<T>* new_node = new (NodeAlloc.allocate(nsz))DQNode<T>(std::move(item));
				left_node->m_prev = new_node;
				new_node->m_next = left_node;
				m_left = new_node;
				m_size.fetch_add(1, std::memory_order_relaxed);
			}
			catch (const std::bad_alloc& ex)
			{
				return false;
			}
		}
		lock.unlock();
		m_pop_cond.notify_one();
		return true;
	}

	template <typename T>
	bool Deque<T>::pop_left(T& target)
	{
		std::unique_lock<mutex_t> lock(m_item_mtx);
		while (m_size.load(std::memory_order_relaxed) == 0) m_pop_cond.wait(lock);
		DQNode<T>* left_node = m_left;
		DQNode<T>* right_node = m_right;
		if (left_node == right_node)	//last element to remove
		{
			target = std::move(left_node->m_item);
			m_left = nullptr;
			m_right = nullptr;
			//delete left_node;
			left_node->~DQNode();
			NodeAlloc.deallocate(left_node);
			m_size.fetch_sub(1, std::memory_order_relaxed);
			return true;
		}
		else
		{
			target = std::move(left_node->m_item);
			left_node->m_next->m_prev = nullptr;
			m_left = left_node->m_next;
			//delete left_node;
			left_node->~DQNode();
			NodeAlloc.deallocate(left_node);
			m_size.fetch_sub(1, std::memory_order_relaxed);
			return true;
		}
	}

	template <typename T>
	bool Deque<T>::try_pop_left(T& target)
	{
		std::lock_guard<mutex_t> lock(m_item_mtx);
		DQNode<T>* left_node = m_left;
		DQNode<T>* right_node = m_right;
		if (left_node == nullptr)			//queue is empty
		{
			return false;
		}
		else if (left_node == right_node)	//last element to remove
		{
			target = std::move(left_node->m_item);
			m_left = nullptr;
			m_right = nullptr;
			//delete left_node;	
			left_node->~DQNode();
			NodeAlloc.deallocate(left_node);
			m_size.fetch_sub(1, std::memory_order_relaxed);
			return true;
		}
		else
		{
			target = std::move(left_node->m_item);
			left_node->m_next->m_prev = nullptr;
			m_left = left_node->m_next;
			//delete left_node;
			left_node->~DQNode();
			NodeAlloc.deallocate(left_node);
			m_size.fetch_sub(1, std::memory_order_relaxed);
			return true;
		}
	}

	template <typename T>
	bool Deque<T>::pop_right(T& target)
	{
		std::unique_lock<mutex_t> lock(m_item_mtx);
		while (m_size.load(std::memory_order_relaxed) != 0) m_pop_cond.wait();
		DQNode<T>* left_node = m_left;
		DQNode<T>* right_node = m_right;
	else if (right_node == left_node) //last element
	{
		target = std::move(right_node->m_item);
		m_right = nullptr;
		m_left = nullptr;
		//delete right_node;
		right_node->~DQNode();
		NodeAlloc.deallocate(right_node);
		m_size.fetch_sub(1, std::memory_order_relaxed);
		return true;
	}
	else
	{
		target = std::move(right_node->m_item);
		right_node->m_prev->m_next = nullptr;
		m_right = right_node->m_prev;
		//delete right_node;
		right_node->~DQNode();
		NodeAlloc.deallocate(right_node);
		m_size.fetch_sub(1, std::memory_order_relaxed);
		return true;
	}
	}

	template <typename T>
	bool Deque<T>::try_pop_right(T& target)
	{
		std::lock_guard<mutex_t> lock(m_item_mtx);
		DQNode<T>* left_node = m_left;
		DQNode<T>* right_node = m_right;
		if (right_node == nullptr)		//queue is empty
		{
			return false;
		}
		else if (right_node == left_node) //last element
		{
			target = std::move(right_node->m_item);
			m_right = nullptr;
			m_left = nullptr;
			//delete right_node;
			right_node->~DQNode();
			NodeAlloc.deallocate(right_node);
			m_size.fetch_sub(1, std::memory_order_relaxed);
			return true;
		}
		else
		{
			target = std::move(right_node->m_item);
			right_node->m_prev->m_next = nullptr;
			m_right = right_node->m_prev;
			//delete right_node;
			right_node->~DQNode();
			NodeAlloc.deallocate(right_node);
			m_size.fetch_sub(1, std::memory_order_relaxed);
			return true;
		}
	}

	template <typename T>
	size_t Deque<T>::size()
	{
		return m_size.load(std::memory_order_relaxed);
	}

	template <typename T>
	bool Deque<T>::empty()
	{
		if (m_size.load(std::memory_order_relaxed) == 0)
			return true;
		return false;
	}

	template <typename T>
	void Deque<T>::clear()
	{
		std::lock_guard<mutex_t> lock(m_item_mtx);
		DQNode<T>* left_node = m_left;
		while (left_node != nullptr)
		{
			DQNode<T>* tmp = left_node;
			left_node = left_node->m_next;
			//delete tmp;
			tmp->~DQNode();
			NodeAlloc.deallocate(tmp);
		}
		m_size.store(0, std::memory_order_relaxed);
	}

}
#endif
/** @}*/