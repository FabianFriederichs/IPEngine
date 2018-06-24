/** \addtogroup memory
*  @{
*/

/*!
\file allocators.h
\brief High-level allocators should go here.
*/

#ifndef _ALLOCATORS_H_
#define _ALLOCATORS_H_
#include <type_traits>
#include <IPCore/Memory/MemoryManager.h>
#include <scoped_allocator>
namespace ipengine
{
	/*!
	\brief std compliant allocator.

	std-compliant allocator implementation that uses MemoryManager for all allocations and deallocations.
	The scoped allocator thing needs to be tested, yet.
	*/
	template <typename T>
	class IPAllocator
	{
		template <typename U> class IPAllocator<U>;

		using value_type = T;
		using pointer = T*;

		using propagate_on_container_copy_assignment = std::true_type;
		using propagate_on_container_move_assignment = std::true_type;
		using propagate_on_container_swap = std::true_type;

	public:
		explicit IPAllocator(MemoryManager* manager) :
			m_manager(manager)
		{

		}

		template <typename U>
		IPAllocator(const IPAllocator<U> other)
		{

		}

		pointer allocate(std::size_t n)
		{
			return static_cast<pointer>(m_manager->alloc(n * sizeof(T), 0));
		}

		void deallocate(pointer p, std::size_t n)
		{
			m_manager->free(static_cast<void*>(p), 0);
		}

		template <typename U>
		bool operator ==(const IPAllocator<U>& other)
		{
			return m_manager == other.m_manager;
		}

		template <typename U>
		bool operator !=(const IPAllocator<U>& other)
		{
			return m_manager != other.m_manager;
		}

	private:
		MemoryManager* m_manager;
	};

	template <typename T>
	using IPAlloc = std::scoped_allocator_adaptor<IPAllocator<T>>;
}

#endif

/** @}*/