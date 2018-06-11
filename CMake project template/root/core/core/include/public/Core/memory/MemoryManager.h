#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_
#include <core/core_config.h>
namespace ipengine
{
	//TODO: add logging functionality if core debug/monitoring stuff is ready
	class CORE_API MemoryManager
	{
	public:
		MemoryManager();
		~MemoryManager();

		//use a more sophisticated gp allocator later
		void* alloc(size_t size, size_t alignment = 0); //alignment unused currently
		//add alloc version for aligned memory
		void free(void* ptr, size_t size = 0); //size unused currently

		template <typename T, typename ... ARGS>
		T* alloct(ARGS...);
		
		template <typename T>
		void freet(T*);
	};

	//TODO: stuff

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

