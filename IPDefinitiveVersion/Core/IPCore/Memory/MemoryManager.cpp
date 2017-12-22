#include "MemoryManager.h"

ipengine::MemoryManager::MemoryManager()
{}

ipengine::MemoryManager::~MemoryManager()
{}

void * ipengine::MemoryManager::alloc(size_t size)
{
	return ::operator new(size);
}

void ipengine::MemoryManager::free(void * ptr)
{
	::operator delete(ptr);
}
