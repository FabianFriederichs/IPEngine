/** \addtogroup memory
Implements memory management functionality of the core.
*  @{
*/

/*!
\file MemoryManager.h
*/
#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_
#include <IPCore/core_config.h>
namespace ipengine
{
	/*!
	\brief Realizes memory management.

	MemoryManager provides basic allocation and deallocation on void pointer level
	as well as allocation and deallocation of single type instances via the templated functions.
	At the moment MemoryManager simply forwards the calls to standard new and delete.
	The interface is slightly inconvenient as of now, but the idea that memory shall be allocated
	on a single heap that is managed by the core does work with this class.

	\todo
	\li Implement a paging system to acquire memory from the OS.
	\li Refactor the low-level allocators and integrate them here to form a consistent concept
	\li Create a more convienient interface
	\li Implement a high-level allocator factory here, that can be used to retrieve allocator instances for
		containers
	\li Add logging and monitoring functionality once the DebugMonitor is up and running
			
	*/
	class CORE_API MemoryManager
	{
	public:
		//! Contructor
		MemoryManager();
		//! Destructor
		~MemoryManager();

		/*!
		\brief Allocates size bytes and returns a void pointer to that memory range
		\param[in] size			The size in bytes of the requested memory block
		\param[in] alignment	Currently unused, should be used later to specify alignment requirements
		\returns				Returns a void pointer to the requested memory block or nullptr if the allocation failed.
		*/
		void* alloc(size_t size, size_t alignment = 0);

		/*!
		\brief Deallocates a block of memory
		\param[in] ptr			The pointer to the block of memory that should be deallocated
		\param[in] size			Currently unused
		*/
		void free(void* ptr, size_t size = 0);

		/*!
		\brief Allocates and initializes a block of memory sufficient for an instance of T

		Allocates a block of memory and calls the constructor with the given arguments to initialize the object.
		\tparam T		Type of the object that should be allocated
		\tparam ARGS	Types of the arguments passed to the constructor of T
		\param args		The arguments that are passed to the constructor of T
		\returns		A pointer to an allocated and initialized object of type T
		*/
		template <typename T, typename ... ARGS>
		T* alloct(ARGS... args);
		
		/*!
		\brief Deallocates an object of type T
		\tparam T		Type of the object that should be deallocated
		\param ptr		Pointer to the object that should be deallocated
		*/
		template <typename T>
		void freet(T* ptr);
	};

	template<typename T, typename ... ARGS>
	inline T * MemoryManager::alloct(ARGS... args)
	{
		void* mem = alloc(sizeof(T));
		T* o = new(mem)T(std::forward(args)...);
		return o;
	}

	template<typename T>
	inline void MemoryManager::freet(T * ptr)
	{
		ptr->~T();
		free(ptr);
	}
}
#endif // !_MEMORY_MANAGER_H_

/** @}*/