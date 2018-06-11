#include <core/memory/MemoryManager.h>

ipengine::MemoryManager::MemoryManager()
{}

ipengine::MemoryManager::~MemoryManager()
{}

void * ipengine::MemoryManager::alloc(size_t size, size_t alignment)
{
	return ::operator new(size);
}

void ipengine::MemoryManager::free(void * ptr, size_t size)
{
	::operator delete(ptr);
}
