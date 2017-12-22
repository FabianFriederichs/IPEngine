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

		void* alloc(size_t size);
		void free(void* ptr);
	};

}
#endif // !_MEMORY_MANAGER_H_

