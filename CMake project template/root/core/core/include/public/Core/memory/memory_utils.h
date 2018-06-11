#ifndef _MEMORY_UTILS_H_
#define _MEMORY_UTILS_H_
#include <core/core_config.h>
#include <cassert>
namespace ipengine
{
	//some utility stuff for working with aligned memory

	constexpr size_t getCacheAlignedSize(size_t size)
	{
		return size + ((TS_CACHE_LINE_SIZE - (size & (TS_CACHE_LINE_SIZE - 1))) & (TS_CACHE_LINE_SIZE - 1));
	}

	constexpr size_t getAlignedSize(size_t size, size_t alignment)
	{
		return size + ((alignment - (size & (alignment - 1))) & (alignment - 1));
	}

	constexpr bool isPowerOf2(size_t n)
	{
		return n && (n & (n - 1)) == 0;
	}

	template<typename T>
	class aligned_ptr //TODO: do pointer_traits stuff
	{
	private:
		template <typename T, size_t alignment, typename ... ARGS>
		friend aligned_ptr<T> alloc_aligned(ARGS&& ... args);

		template <typename T>
		friend void free_aligned(aligned_ptr<T>& ptr);

		template <typename T, size_t alignment>
		friend aligned_ptr<T> alloc_aligned_array(size_t n);

		template <typename T>
		friend void free_aligned_array(aligned_ptr<T>& ptr);

		T* ptr;
		void* mem_ptr;
		size_t array_length;
		size_t elem_size;
	public:
		aligned_ptr() :
			ptr(nullptr),
			mem_ptr(nullptr),
			array_length(0),
			elem_size(0)
		{

		}

		aligned_ptr(std::nullptr_t p) :
			ptr(nullptr),
			mem_ptr(nullptr),
			array_length(0),
			elem_size(0)
		{

		}

		~aligned_ptr()
		{

		}

		aligned_ptr(const aligned_ptr<T>& other) :
			ptr(other.ptr),
			mem_ptr(other.mem_ptr),
			array_length(other.array_length),
			elem_size(other.elem_size)
		{

		}

		aligned_ptr(aligned_ptr<T>&& other) :
			ptr(other.ptr),
			mem_ptr(other.mem_ptr),
			array_length(other.array_length),
			elem_size(other.elem_size)
		{
			other.invalidate();
		}

		aligned_ptr(T* _ptr, void* _mem) :
			ptr(_ptr),
			mem_ptr(_mem),
			array_length(0),
			elem_size(0)
		{

		}

		aligned_ptr(T* _ptr, void* _mem, size_t n, size_t es) :
			ptr(_ptr),
			mem_ptr(_mem),
			array_length(n),
			elem_size(es)
		{

		}

		aligned_ptr<T>& operator=(const aligned_ptr<T>& other)
		{
			if (this == &other)
				return *this;
			ptr = other.ptr;
			mem_ptr = other.mem_ptr;
			array_length = other.array_length;
			elem_size = other.elem_size;
			return *this;
		}

		aligned_ptr<T>& operator=(aligned_ptr<T>&& other)
		{
			if (this == &other)
				return *this;
			ptr = other.ptr;
			mem_ptr = other.mem_ptr;
			array_length = other.array_length;
			elem_size = other.elem_size;
			other.invalidate();
			return *this;
		}

		T* operator->()
		{
			assert(ptr != nullptr);
			return ptr;
		}

		const T* operator->() const
		{
			assert(ptr != nullptr);
			return ptr;
		}

		T& operator*()
		{
			assert(ptr != nullptr);
			return *ptr;
		}

		const T& operator*() const
		{
			assert(ptr != nullptr);
			return *ptr;
		}

		T* get()
		{
			return ptr;
		}

		const T* get() const
		{
			return ptr;
		}

		const T& operator[](size_t index) const
		{
			assert(array_length > 0 && ptr != nullptr);
			uintptr_t resptr = reinterpret_cast<uintptr_t>(ptr) + (index * elem_size);
			return *reinterpret_cast<T*>(resptr);
		}

		T& operator[](size_t index)
		{
			assert(array_length > 0 && ptr != nullptr);
			uintptr_t resptr = reinterpret_cast<uintptr_t>(ptr) + (index * elem_size);
			return *reinterpret_cast<T*>(resptr);
		}

		void invalidate()
		{
			ptr = nullptr;
			mem_ptr = nullptr;
		}

		explicit operator bool()
		{
			return ptr != nullptr && mem_ptr != nullptr;
		}

		friend bool operator==(const aligned_ptr<T>& lhs, const aligned_ptr<T>& rhs);
		friend bool operator!=(const aligned_ptr<T>& lhs, const aligned_ptr<T>& rhs);
	};

	template <typename T>
	bool operator==(const aligned_ptr<T>& lhs, const aligned_ptr<T>& rhs)
	{
		return lhs.mem_ptr == rhs.mem_ptr && lhs.ptr == rhs.ptr;
	}

	template <typename T>
	bool operator!=(const aligned_ptr<T>& lhs, const aligned_ptr<T>& rhs)
	{
		return !(lhs.mem_ptr == rhs.mem_ptr && lhs.ptr == rhs.ptr);
	}

	template <typename T, size_t alignment, typename ... ARGS>
	inline aligned_ptr<T> alloc_aligned(ARGS&& ... args)
	{
		static_assert(isPowerOf2(alignment), "alignment template parameter has to be a power of 2");
		size_t space = sizeof(T) + alignment - 1;
		void* ptr = ::operator new(space);
		void* aligned = ptr;
		if (std::align(alignment, sizeof(T), aligned, space) == nullptr)
		{
			::operator delete(ptr);
			throw std::bad_alloc();
		}
		new(aligned)T(std::forward<ARGS>(args)...);
		return aligned_ptr<T>(reinterpret_cast<T*>(aligned), ptr);
	}

	template <typename T>
	inline void free_aligned(aligned_ptr<T>& ptr)
	{		
		if (ptr.ptr != nullptr)
		{
			assert(ptr.array_length == 0);
			ptr.ptr->T::~T();
			ptr.ptr = nullptr;
		}
		if (ptr.mem_ptr != nullptr)
		{
			assert(ptr.array_length == 0);
			::operator delete(ptr.mem_ptr);
			ptr.mem_ptr = nullptr;
		}
	}

	//TODO: Array versions!
	template <typename T, size_t alignment>
	inline aligned_ptr<T> alloc_aligned_array(size_t n)
	{
		assert(n > 0);
		static_assert(isPowerOf2(alignment), "alignment template parameter has to be a power of 2");
		size_t alignedElemSize = getAlignedSize(sizeof(T), alignment);
		size_t space = alignedElemSize * n + alignment - 1;
		void* ptr = ::operator new(space);
		void* aligned = ptr;
		if (std::align(alignment, alignedElemSize * n, aligned, space) == nullptr)
		{
			::operator delete(ptr);
			throw std::bad_alloc();
		}
		for (uintptr_t p = reinterpret_cast<uintptr_t>(aligned); p < (reinterpret_cast<uintptr_t>(aligned) + alignedElemSize * n); p += alignedElemSize)
		{
			new(reinterpret_cast<void*>(p))T;
		}
		return aligned_ptr<T>(reinterpret_cast<T*>(aligned), ptr, n, alignedElemSize);
	}

	template <typename T>
	inline void free_aligned_array(aligned_ptr<T>& ptr)
	{		
		if (ptr.ptr != nullptr)
		{
			assert(ptr.array_length > 0);
			for (uintptr_t p = reinterpret_cast<uintptr_t>(ptr.ptr); p < (reinterpret_cast<uintptr_t>(ptr.ptr) + ptr.array_length * ptr.elem_size); p += ptr.elem_size)
			{
				reinterpret_cast<T*>(p)->T::~T();
			}
			ptr.ptr = nullptr;
		}
		if (ptr.mem_ptr != nullptr)
		{
			assert(ptr.array_length > 0);
			::operator delete(ptr.mem_ptr);
			ptr.mem_ptr = nullptr;
		}		
	}
}
#endif
