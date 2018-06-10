#pragma once
#include <type_traits>
#include <IPCore/Util/spinlock.h>

namespace ipengine {

	template <typename T>
	class stack
	{
		static_assert(std::is_trivially_copyable<T>::value, "Types used with this basic stack must be trivially copyable");
		size_t m_logsz;
		T* m_arr;
		size_t m_index;
	public:
		stack();
		stack(const stack& other);
		stack(stack&& other);
		~stack();

		void push(T item);
		T& pop();
		T& top();
		size_t size();
	private:
		void grow();
	};

	template<typename T>
	inline stack<T>::stack() :
		m_logsz(10),
		m_index(0),
		m_arr(new T[size_t(1) << size_t(10)])
	{
	}

	template<typename T>
	inline stack<T>::stack(const stack & other) :
		m_logsz(other.m_logsz),
		m_index(other.m_index),
		m_arr(new T[1 << other.m_logsz])
	{
		memcpy_s(m_arr, size_t(1) << m_logsz, other.m_arr, size_t(1) << other.m_logsz);
	}

	template<typename T>
	inline stack<T>::stack(stack && other) :
		m_logsz(other.m_logsz),
		m_index(other.m_index),
		m_arr(other.m_arr)
	{
		other.m_arr = nullptr;
		other.m_index = 0;
		other.m_logsz = 0;
	}

	template<typename T>
	inline stack<T>::~stack()
	{
		if (m_arr != nullptr)
			delete[] m_arr;
	}

	template<typename T>
	inline void stack<T>::push(T item)
	{
		if (m_index >= (size_t(1) << m_logsz))
		{
			grow();
		}
		m_arr[m_index] = item;
		++m_index;
	}

	template<typename T>
	inline T& stack<T>::pop()
	{
		return m_arr[--m_index];
	}

	template<typename T>
	inline T & stack<T>::top()
	{
		return m_arr[m_index - 1];
	}

	template<typename T>
	inline size_t stack<T>::size()
	{
		return m_index;
	}

	template<typename T>
	inline void stack<T>::grow()
	{
		T* old = m_arr;
		m_arr = new T[size_t(1) << (m_logsz * 2)];
		//std::copy(old, old + m_index, m_arr);
		memcpy_s(m_arr, size_t(1) << m_logsz, old, size_t(1) << m_logsz);
		m_logsz = m_logsz * 2;
		delete[] old;
	}

	template <typename T>
	class spinlock_stack
	{
		static_assert(std::is_trivially_copyable<T>::value, "Types used with this basic spinlock_stack must be trivially copyable");
		size_t m_logsz;
		T* m_arr;
		size_t m_index;
		YieldingSpinLock<5000> lock;
	public:
		spinlock_stack();
		spinlock_stack(const spinlock_stack& other);
		spinlock_stack(spinlock_stack&& other);
		~spinlock_stack();

		void push(T item);
		T& pop();
		T& top();
		size_t size();
	private:
		void grow();
	};

	template<typename T>
	inline spinlock_stack<T>::spinlock_stack() :
		m_logsz(10),
		m_index(0),
		m_arr(new T[size_t(1) << size_t(10)])
	{
	}

	template<typename T>
	inline spinlock_stack<T>::spinlock_stack(const spinlock_stack & other) :
		m_logsz(other.m_logsz),
		m_index(other.m_index),
		m_arr(new T[size_t(1) << other.m_logsz])
	{
		memcpy_s(m_arr, size_t(1) << m_logsz, other.m_arr, size_t(1) << other.m_logsz);
	}

	template<typename T>
	inline spinlock_stack<T>::spinlock_stack(spinlock_stack && other) :
		m_logsz(other.m_logsz),
		m_index(other.m_index),
		m_arr(other.m_arr)
	{
		other.m_arr = nullptr;
		other.m_index = 0;
		other.m_logsz = 0;
	}

	template<typename T>
	inline spinlock_stack<T>::~spinlock_stack()
	{
		if (m_arr != nullptr)
			delete[] m_arr;
	}

	template<typename T>
	inline void spinlock_stack<T>::push(T item)
	{
		std::lock_guard<YieldingSpinLock<5000>> lg(lock);
		if (m_index >= (size_t(1) << m_logsz))
		{
			grow();
		}
		m_arr[m_index] = item;
		++m_index;
	}

	template<typename T>
	inline T& spinlock_stack<T>::pop()
	{
		std::lock_guard<YieldingSpinLock<5000>> lg(lock);
		return m_arr[--m_index];
	}

	template<typename T>
	inline T & spinlock_stack<T>::top()
	{
		std::lock_guard<YieldingSpinLock<5000>> lg(lock);
		return m_arr[m_index - 1];
	}

	template<typename T>
	inline size_t spinlock_stack<T>::size()
	{
		std::lock_guard<YieldingSpinLock<5000>> lg(lock);
		return m_index;
	}

	template<typename T>
	inline void spinlock_stack<T>::grow()
	{
		T* old = m_arr;
		m_arr = new T[size_t(1) << (m_logsz * 2)];
		//std::copy(old, old + m_index, m_arr);
		memcpy_s(m_arr, size_t(1) << m_logsz, old, size_t(1) << m_logsz);
		m_logsz = m_logsz * 2;
		delete[] old;
	}
}
