#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_
#include <core_config.h>
namespace ipengine
{
	//TODO: add logging functionality if core debug/monitoring stuff is ready
	class CORE_API MemoryManager
	{
	public:
		MemoryManager();
		~MemoryManager();

		//use a more sophisticated gp allocator later
		void* alloc(size_t size, size_t alignment = 0);
		//add alloc version for aligned memory
		void free(void* ptr, size_t size = 0);
	};
}
#endif // !_MEMORY_MANAGER_H_

