/** \addtogroup memory
*  @{
*/

/*!
\file memory_utils.h
\brief Aligned allocation helpers
*/
#ifndef _MEMORY_UTILS_H_
#define _MEMORY_UTILS_H_
#include <IPCore/core_config.h>
#include <cassert>
namespace ipengine
{
	/*!
	\brief Computes the nearest size to the size parameter that is a multiple of a cache line

	\param[in] size		Input size
	\returns			The nearest size that is a multiple of a cache line
	*/
	constexpr size_t getCacheAlignedSize(size_t size)
	{
		return size + ((TS_CACHE_LINE_SIZE - (size & (TS_CACHE_LINE_SIZE - 1))) & (TS_CACHE_LINE_SIZE - 1));
	}

	/*!
	\brief Computes the nearest size to the size parameter that is a multiple of alignment

	\param[in] size			Input size
	\param[in] alignment	The desired alignment
	\returns				The nearest size that is a multiple of a alignment
	*/
	constexpr size_t getAlignedSize(size_t size, size_t alignment)
	{
		return size + ((alignment - (size & (alignment - 1))) & (alignment - 1));
	}

	/*!
	\brief Checks if size n is a power of two

	\param[in] n	Some size
	\returns		Returns true if n is a power of two
	*/
	constexpr bool isPowerOf2(size_t n)
	{
		return n && (n & (n - 1)) == 0;
	}

	/*!
	\brief Pointer-like type that holds an aligned pointer to some object

	When stricter-than standard alignment is needed, a slightly larger block of memory is allocated and the
	resulting pointer is shifted by an offset so that the pointer's value meets the alignment requirement.
	Although, the original pointer is needed to deallocate the memory.
	This is what this class does: It holds the aligned object pointer and the original pointer to
	the allocated block of memory.

	In addition to that it provides an index operator which allows for convenient access to arrays of aligned
	objects.

	\tparam T	Type of the object
	*/
	template<typename T>
	class aligned_ptr
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
		/*!
		\brief Default constructor
		*/
		aligned_ptr() :
			ptr(nullptr),
			mem_ptr(nullptr),
			array_length(0),
			elem_size(0)
		{

		}

		/*!
		\brief Constructor that takes std::nullptr_t

		This signature is a requirement for std conform fancy pointers.
		*/
		aligned_ptr(std::nullptr_t p) :
			ptr(nullptr),
			mem_ptr(nullptr),
			array_length(0),
			elem_size(0)
		{

		}

		/*!
		\brief Destructor

		This is NOT a smart pointer. If the object is destructed before deallocation of the pointee,
		the memory is leaked.
		*/
		~aligned_ptr()
		{

		}

		/*!
		\brief Copy constructor

		Copies the pointer.
		*/
		aligned_ptr(const aligned_ptr<T>& other) :
			ptr(other.ptr),
			mem_ptr(other.mem_ptr),
			array_length(other.array_length),
			elem_size(other.elem_size)
		{

		}

		/*!
		\brief Move constructor
		*/
		aligned_ptr(aligned_ptr<T>&& other) :
			ptr(other.ptr),
			mem_ptr(other.mem_ptr),
			array_length(other.array_length),
			elem_size(other.elem_size)
		{
			other.invalidate();
		}

		/*!
		\brief Initializes the aligned_ptr with object pointer and memory pointer.
		*/
		aligned_ptr(T* _ptr, void* _mem) :
			ptr(_ptr),
			mem_ptr(_mem),
			array_length(0),
			elem_size(0)
		{

		}

		/*!
		\brief Initializes the array version of the pointer.
		*/
		aligned_ptr(T* _ptr, void* _mem, size_t n, size_t es) :
			ptr(_ptr),
			mem_ptr(_mem),
			array_length(n),
			elem_size(es)
		{

		}

		/*!
		\brief Copy assignment
		*/
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

		/*!
		\brief Move assignment
		*/
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

		/*!
		\brief Member access operator

		Provides direct access to the pointee's members.
		*/
		T* operator->()
		{
			assert(ptr != nullptr);
			return ptr;
		}

		/*!
		\brief Const member access operator

		Provides access to the pointee's const members.
		*/
		const T* operator->() const
		{
			assert(ptr != nullptr);
			return ptr;
		}

		/*!
		\brief Dereference operator

		Returns a reference to the pointee.
		*/
		T& operator*()
		{
			assert(ptr != nullptr);
			return *ptr;
		}

		/*!
		\brief Const dereference operator

		Returns a const reference to the pointee.
		*/
		const T& operator*() const
		{
			assert(ptr != nullptr);
			return *ptr;
		}

		/*!
		\brief Retrieve the raw object pointer

		Returns the raw pointer to the pointee.
		*/
		T* get()
		{
			return ptr;
		}

		/*!
		\brief Retreive the const raw object pointer

		Returns the const raw pointer to the pointee.
		*/
		const T* get() const
		{
			return ptr;
		}

		/*!
		\brief Const subscript operator

		Access const array elements. Triggers an assertion if the pointee is not an array.
		Don't use pointer arithmetics on the raw pointer to access elements! Instead ONLY use this operator.
		(The size of an element is in general not equal to the size of T)
		*/
		const T& operator[](size_t index) const
		{
			assert(array_length > 0 && ptr != nullptr);
			uintptr_t resptr = reinterpret_cast<uintptr_t>(ptr) + (index * elem_size);
			return *reinterpret_cast<T*>(resptr);
		}

		/*!
		\brief Subscript operator

		Access array elements. Triggers an assertion if the pointee is not an array.
		Don't use pointer arithmetics on the raw pointer to access elements! Instead ONLY use this operator.
		(The size of an element is in general not equal to the size of T)
		*/
		T& operator[](size_t index)
		{
			assert(array_length > 0 && ptr != nullptr);
			uintptr_t resptr = reinterpret_cast<uintptr_t>(ptr) + (index * elem_size);
			return *reinterpret_cast<T*>(resptr);
		}

		/*!
		\brief Sets object pointer and memory pointer to nullptr.
		*/
		void invalidate()
		{
			ptr = nullptr;
			mem_ptr = nullptr;
		}

		/*!
		\brief Conversion operator to bool.

		True if object pointer and memory pointer are != nullptr.
		*/
		explicit operator bool()
		{
			return ptr != nullptr && mem_ptr != nullptr;
		}

		friend bool operator==(const aligned_ptr<T>& lhs, const aligned_ptr<T>& rhs);
		friend bool operator!=(const aligned_ptr<T>& lhs, const aligned_ptr<T>& rhs);
	};

	/*!
	\brief Two aligned_ptr objects are equal if the two stored pointers are.
	*/
	template <typename T>
	bool operator==(const aligned_ptr<T>& lhs, const aligned_ptr<T>& rhs)
	{
		return lhs.mem_ptr == rhs.mem_ptr && lhs.ptr == rhs.ptr;
	}

	/*!
	\brief Two aligned_ptr objects are unequal if the two stored pointers are.
	*/
	template <typename T>
	bool operator!=(const aligned_ptr<T>& lhs, const aligned_ptr<T>& rhs)
	{
		return !(lhs.mem_ptr == rhs.mem_ptr && lhs.ptr == rhs.ptr);
	}

	/*!
	\brief Allocates and initializes an aligned instance of T

	\tparam T			Type of the object
	\tparam alignment	Desired alignment (must be a power of two)
	\tparam ARGS		Types of the arguments passed to T's constructor
	\param args			Arguments passed to T's constructor

	\returns			Returns an aligned_ptr<T> object that acts as pointer to an
						object of type T that meets the alignment requirement specified
						by the alignment template parameter.
	*/
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

	/*!
	\brief Destructs an aligned instance of type T and deallocates its memory.

	Destructs the object pointed to by the object pointer.
	Deallocates the memory pointed to by the memory pointer.

	\tparam T			Type of the object
	\param ptr			The aligned_ptr object
	*/
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

	/*!
	\brief Allocates and initializes an array of aligned instances of T

	\tparam T			Type of the object
	\tparam alignment	Desired alignment (must be a power of two)
	\param n			Number of elements

	\returns			Returns an aligned_ptr<T> object that acts as pointer to an
						array of objects of type T that meet the alignment requirement specified
						by the alignment template parameter.
	*/
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

	/*!
	\brief Destructs an array of aligned instances of type T and deallocates its memory.

	Destructs all elements of the array.
	Deallocates the memory pointed to by the memory pointer.

	\tparam T			Type of the object
	\param ptr			The aligned_ptr object whose pointee should be deallocated
	*/
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
/** @}*/