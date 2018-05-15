#ifndef _LOW_LEVEL_ALLOCATORS_H_
#define _LOW_LEVEL_ALLOCATORS_H_
#include <atomic>
#include <vector>
#include <IPCore/Util/spinlock.h>
#include <chrono>
#include <IPCore/core_config.h>
#include <IPCore/Memory/memory_utils.h>

namespace ipengine {
	//TODO: provide some instantiations of those in core
	//------------------------------- FIXED SIZE ELEMENT FREE LISTS -----------------------------------------------------------------------

	//Low level allocators for fixed sized elements. One simple, fast and not threadsafe
	//the other slightly slower, threadsafe and static (not instantiable!). This is important! Don't alloc and free
	//memory with the second version across different translation units!!

	//Basic Free List allocator for fixed block size with arbitrary alignment, not threadsafe
	template <size_t alignment, size_t blocksize, size_t blocksperchunk>
	class FreeList
	{
		static_assert(isPowerOf2(alignment) || alignment == 0, "alignment parameter must be a power of 2 or 0 if no specific aligmnent is desired.");
		static_assert(blocksize > 0, "blocksize must be greater than 0.");
		static_assert(blocksperchunk > 0, "blocksperchunk must be greater than 0.");

		typedef struct node
		{
			node* next;
		} node;

		typedef struct chunk
		{
			void* memptr;
			void* alignedaddr;
			node* head;
			node* last;
		} chunk;

		node* head;
		size_t chunk_count;
		size_t aligned_blocksize;
		size_t aligned_chunksize;
		std::vector<chunk> chunks;
		bool initialized;

	private:
		bool allocate_chunk(chunk& c)
		{
			if (!initialized)
				return false;
			void* chunkmem;
			c.memptr = nullptr;
			try
			{
				chunkmem = ::operator new(aligned_chunksize);
			}
			catch (const std::bad_alloc&)
			{
				return false;
			}
			uintptr_t unalignedptr = reinterpret_cast<uintptr_t>(chunkmem);
			uintptr_t align = static_cast<uintptr_t>(alignment);
			uintptr_t offset = (align > 0 ? ((align - (unalignedptr & (align - 1))) & (align - 1)) : 0);
			uintptr_t alignedptr = unalignedptr + offset;
			c.memptr = chunkmem;
			c.alignedaddr = reinterpret_cast<void*>(alignedptr);
			c.head = nullptr;
			return true;
		}

		bool setup_chunk(chunk& c)
		{
			if (!initialized)
				return false;
			if (c.memptr == nullptr)
				return false;

			//start at aligned addr and placement-new nodes on the addresses
			//first node becomes head

			//construct first node at alignedaddr
			node* nptr = new(c.alignedaddr) node;
			node* first = nptr;

			for (size_t i = 1; i < blocksperchunk; ++i)
			{
				//construct next node at an offset of aligned_blocksize
				nptr->next = new(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(nptr) + static_cast<uintptr_t>(aligned_blocksize))) node;
				//update run variable
				nptr = nptr->next;
				//set the next-pointer to nullptr to mark the new end of the list
				nptr->next = nullptr;
			}

			//set head of the chunk to the first created node at alignedaddr
			c.head = first;
			c.last = nptr;
			return true;
		}

		void deallocate_chunk(chunk& c)
		{
			if (c.memptr != nullptr)
			{
				delete c.memptr;
				c.memptr = nullptr;
				c.alignedaddr = nullptr;
				c.head = nullptr;
				c.last = nullptr;
			}
		}

		bool addChunk()
		{
			//if head isn't nullpointer, the free list is not empty so adding a new block would corrupt the list.
			if (head != nullptr)
				return false;
			chunk c;
			//allocate the chunk
			if (!allocate_chunk(c))
				return false;
			//and setup the nodelist inside
			setup_chunk(c);
			//push the new chunk onto our chunk vector
			chunks.push_back(c);
			//increse chunk count
			++chunk_count;
			head = chunks[chunks.size() - 1].head;
			return true;
		}

	public:
		FreeList() :
			head(nullptr),
			chunk_count(0),
			aligned_blocksize(0),
			aligned_chunksize(0),
			chunks(),
			initialized(false)
		{
		}

		~FreeList()
		{
			for (auto& c : chunks)
			{
				deallocate_chunk(c);
			}
		}

		bool initialize()
		{
			if (initialized)
				return true;
			size_t correctedblocksize = (blocksize < sizeof(node) ? sizeof(node) : blocksize);
			//calculate offset so that adjacent blocks in a chunk are still aligned if the chunk itself is
			size_t sizeoffset = (alignment > 0 ? ((alignment - (correctedblocksize & (alignment - 1))) & (alignment - 1)) : 0);
			//aligned blocksize
			aligned_blocksize = correctedblocksize + sizeoffset;
			//aligned_chunksize is blockperchunk * aligned_blocksize plus additional alignment - 1 bytes to align the chunk itself
			aligned_chunksize = aligned_blocksize * blocksperchunk + (alignment > 0 ? alignment - 1 : 0);
			initialized = true;

			if (!addChunk())
				return false;

			return true;
		}

		void* allocate(size_t size)
		{
			if (size > blocksize)
				throw std::bad_alloc();
			if (!initialized)
				throw std::logic_error("FreeList allocator is not initialized.");

			if (head != nullptr)
			{
				void* ptr = reinterpret_cast<void*>(head);
				head = head->next;
				return ptr;
			}
			else
			{
				if (!addChunk())
					throw std::bad_alloc();

				void* ptr = reinterpret_cast<void*>(head);
				head = head->next;
				return ptr;
			}
		}

		void deallocate(void* ptr)
		{
			if (!initialized)
				throw std::logic_error("FreeList allocator is not initialized.");

			node* reclaimed = new(ptr)node;
			reclaimed->next = head;
			head = reclaimed;
		}

		bool isFromList(void* address)
		{
			for (int i = 0; i < chunks.size(); i++)
			{
				if (address >= reinterpret_cast<void*>(chunks[i].head) && address <= reinterpret_cast<void*>(chunks[i].last)) // additionally check if pointer is a multiple of block size?
					return true;
			}
			return false;
		}
	};


	//TODO: There is a bug when blocksperchunk == 1. Also bad performance of our Central Free List.
	//Probably making batch and chunk arrays lockfree will fix this.

	template <size_t alignment, size_t blocksize, size_t blocksperchunk>
	class ThreadSafeFreeList
	{
		static_assert(isPowerOf2(alignment) || alignment == 0, "alignment parameter must be a power of 2 or 0 if no specific aligmnent is desired.");
		static_assert(blocksize > 0, "blocksize must be greater than 0.");
		static_assert(blocksperchunk > 0, "blocksperchunk must be greater than 0.");

	private:
		//internal types
		class CentralFreeList
		{
		public:
			using node = struct node
			{
				node* next;
			};

			using batch = struct alignas(IP_CACHE_LINE_SIZE)batch
			{
				node* first;
				//node* last;
				size_t size;
			};
		private:
			typedef struct alignas(IP_CACHE_LINE_SIZE)chunk
			{
				void* memptr;
				void* alignedaddr;
				node* head;
				//node* last;
			} chunk;

			size_t chunk_count;
			size_t aligned_blocksize;
			size_t aligned_chunksize;
			std::vector<chunk> chunks;
			std::vector<batch> batches;//use boost lockfree stack
			bool initialized;
			using mutex_t = YieldingSpinLock<5000>;
			mutex_t mtx;

			//TODO: we need a method that sorts al blocks after chunks and deletes chunks as they are completely free again. could be very costly, so use this in 
			//exceptional situations

		private:
			bool allocate_chunk(chunk& c)
			{
				if (!initialized)
					return false;
				void* chunkmem;
				c.memptr = nullptr;
				try
				{
					chunkmem = ::operator new(aligned_chunksize);
					//std::cout << "Allocated new chunk\n";
				}
				catch (const std::bad_alloc&)
				{
					return false;
				}
				uintptr_t unalignedptr = reinterpret_cast<uintptr_t>(chunkmem);
				uintptr_t align = static_cast<uintptr_t>(alignment);
				uintptr_t offset = (align > 0 ? ((align - (unalignedptr & (align - 1))) & (align - 1)) : 0);
				uintptr_t alignedptr = unalignedptr + offset;
				c.memptr = chunkmem;
				c.alignedaddr = reinterpret_cast<void*>(alignedptr);
				c.head = nullptr;
				return true;
			}

			bool setup_chunk(chunk& c)
			{
				if (!initialized)
					return false;
				if (c.memptr == nullptr)
					return false;

				node* nptr = new(c.alignedaddr) node;
				node* first = nptr;

				for (size_t i = 1; i < blocksperchunk; ++i)
				{
					//construct next node at an offset of aligned_blocksize
					nptr->next = reinterpret_cast<node*>(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(nptr) + static_cast<uintptr_t>(aligned_blocksize)));
					//update run variable
					nptr = nptr->next;
					//set the next-pointer to nullptr to mark the new end of the list
					nptr->next = nullptr;
				}

				//set head of the chunk to the first created node at alignedaddr
				c.head = first;
				//c.last = nptr;
				return true;
			}

			void deallocate_chunk(chunk& c)
			{
				if (c.memptr != nullptr)
				{
					delete c.memptr;
					c.memptr = nullptr;
					c.alignedaddr = nullptr;
					c.head = nullptr;
				}
			}

			bool addChunk()
			{
				chunk c;
				//allocate the chunk
				if (!allocate_chunk(c))	//everything is fine
					return false;
				//and setup the nodelist inside
				setup_chunk(c);
				//push the new chunk onto our chunk vector
				chunks.push_back(c);	//we have to keep track of our total chunks allocated from the os. in the end we have to free them all
										//increse chunk count
				++chunk_count;	//potentially unneccessary

				batches.push_back(batch{ c.head, blocksperchunk });
				//std::cout << "Allocated new chunk\n";
				return true;
			}

		public:

			CentralFreeList() :
				//head(nullptr),
				chunk_count(0),
				aligned_blocksize(0),
				aligned_chunksize(0),
				chunks(),
				initialized(false)
			{
				initialize();
			}

			~CentralFreeList()
			{
				for (auto& c : chunks)
				{
					deallocate_chunk(c);
				}
			}

			void reserve(size_t n)
			{
				batches.reserve(n);
				chunks.reserve(n);
				for (size_t i = 0; i < n; i++)
				{
					if (!addChunk())
						throw std::bad_alloc();
				}
			}

			bool initialize()
			{
				if (initialized)
					return true;
				size_t correctedblocksize = (blocksize < sizeof(node) ? sizeof(node) : blocksize);
				//calculate offset so that adjacent blocks in a chunk are still aligned if the chunk itself is
				size_t sizeoffset = (alignment > 0 ? ((alignment - (correctedblocksize & (alignment - 1))) & (alignment - 1)) : 0);
				//aligned blocksize
				aligned_blocksize = correctedblocksize + sizeoffset;
				//aligned_chunksize is blockperchunk * aligned_blocksize plus additional alignment - 1 bytes to align the chunk itself
				aligned_chunksize = aligned_blocksize * blocksperchunk + (alignment > 0 ? alignment - 1 : 0);
				initialized = true;

				//if (!addChunk())
				//	return false;

				return true;
			}

			batch pullBatch()
			{
				std::lock_guard<mutex_t> lock(mtx);
				if (batches.size() == 0)
				{
					if (!addChunk())
						throw std::bad_alloc();
				}
				batch b = batches.back();
				batches.pop_back();
				return b;
			}

			void pushBatch(const batch& b)
			{
				//std::cout << "pullbatch\n";
				std::lock_guard<mutex_t> lock(mtx);
				batches.push_back(b);
			}
		};

		using CList = CentralFreeList;

		class ThreadCache
		{
		public:
			//ctors/dtor

			//members

			size_t count; //if count > blocksperchunk save position into split. and update splitend on every incoming free.  if count is then > 2*blocksperchunk push the batch beginning with split back on cl.
			size_t splitcount;
			typename CList::node* head;	//if splitlist is not pushed back yet and head is nullptr, head = splitstart and splitstart = splitend = nullptr
			typename CList::node* split;
			CList& clst;

			ThreadCache(CList& clst) :
				count(0),
				splitcount(0),
				head(nullptr),
				split(nullptr),
				clst(clst)
			{

			}

			~ThreadCache()
			{
				if (count > 0 && head != nullptr)
				{
					clst.pushBatch(typename CList::batch{ head, count });
					//std::cout << "Active count: " << count << "\n";
				}
				if (splitcount > 0 && split != nullptr)
				{
					clst.pushBatch(typename CList::batch{ split, splitcount });
					//std::cout << "Split count: " << splitcount << "\n";
				}
			}

			void* allocate(size_t size)
			{
				if (head == nullptr)
				{
					typename CList::batch b = clst.pullBatch();
					//std::cout << "Pulled a new batch\n";					
					head = b.first;
					count = b.size;
				}

				void* p = static_cast<void*>(head);
				head = head->next;
				--count;
				return p;

				/*
				if (head == nullptr)
				{
					//++tcecount;
					//return nullptr;
					//std::cout << "Threadcache is empty\n";
					if (split != nullptr)
					{
						//std::cout << "Swapped in splitlist\n";
						head = split;
						split = nullptr;
						count = splitcount;
						splitcount = 0;
					}
					else
					{
						//++pullcount;
						typename CList::batch b = clst.pullBatch();
						//std::cout << "Pulled a new batch\n";
						head = b.first;
						split = nullptr;
						splitcount = 0;
						count = b.size;
					}
				}
				//proceed using head as the new allocated block as usual

				void* p = static_cast<void*>(head);
				head = head->next;
				--count;
				return p;
				*/
			}

			void deallocate(void* ptr)	//something weird here. deallocate is quite slow. figure this out later
			{
				typename CList::node* reclaimed = static_cast<typename CList::node*>(ptr);
				reclaimed->next = head;
				head = reclaimed;
				++count;

				if (count <= blocksperchunk)
					return;

				if (count == blocksperchunk + 1)
				{
					split = head;
					return;
				}

				if (count == 2 * blocksperchunk)
				{
					typename CList::batch b{ split->next, blocksperchunk };
					split->next = nullptr;
					clst.pushBatch(b);
					count -= blocksperchunk;
					return;
				}



				/*
				if (count < blocksperchunk) //put freed elements onto active list
				{
					typename CList::node* reclaimed = static_cast<typename CList::node*>(ptr);
					reclaimed->next = head;
					head = reclaimed;
					++count;
				}
				else
				{
					typename CList::node* reclaimed = static_cast<typename CList::node*>(ptr);
					reclaimed->next = split;
					split = reclaimed;
					++splitcount;

					if (splitcount == blocksperchunk)
					{
						//push the enire splitlist back onto the central list
						typename CList::batch b{ split, splitcount };
						clst.pushBatch(b);
						split = nullptr;
						splitcount = 0;
						//std::cout << "Returned splitlist\n";
					}
				}
				*/
			}
		};


		static CList centralList;
		static thread_local ThreadCache tc;

	public:

		static inline void* allocate(size_t size)
		{
			if (size > blocksize)
				throw std::bad_alloc();
			return tc.allocate(size);
		}

		static inline void deallocate(void* ptr)
		{
			tc.deallocate(ptr);
		}

		static void reservechunks(size_t n)
		{
			centralList.reserve(n);
		}
	};

	template<size_t alignment, size_t blocksize, size_t blocksperchunk>
	typename ThreadSafeFreeList<alignment, blocksize, blocksperchunk>::CList ThreadSafeFreeList<alignment, blocksize, blocksperchunk>::centralList;

	template<size_t alignment, size_t blocksize, size_t blocksperchunk>
	thread_local typename ThreadSafeFreeList<alignment, blocksize, blocksperchunk>::ThreadCache ThreadSafeFreeList<alignment, blocksize, blocksperchunk>::tc(ThreadSafeFreeList<alignment, blocksize, blocksperchunk>::centralList);

	//---------------------------------------------------------------------------------------------------------------------------------

	//TODO: some other memory allocation strategies:
	//- stack allocator
	//- linear allocator
	//- some general purpose thingy
	//integrate them into the MemoryManager class later
	

}
#endif
