/** \addtogroup memory
*  @{
*/

/*!
\file lowlevel_allocators.h
\brief Defines some special allocators
*/
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

	/*!
	\brief Free list allocator strategy implementation for fixed block sizes. Not thread safe.

	This class implements a basic free list allocator. Chunks of memory are allocated on demand and
	split into blocks. A block can either be occupied (through an allocation) or free.
	A free block contains a pointer to the next free block, i.e. the free blocks form
	a in-memory singly linked list. Allocations pop blocks and deallocations push blocks
	from and onto that list. If the list is empty, a new chunk is allocated and split into
	blocks, the resulting list is merged with the currently empty list.
	Currently the chunks are never freed until the FreeList object is destroyed.

	\tparam alignment		The desired alignment for each block
	\tparam blocksize		The desired block size (note that this sets the maximum number of bytes that can be allocated at once)
	\tparam	blocksperchunk	Defines the size of a chunk
	*/
	template <size_t alignment, size_t blocksize, size_t blocksperchunk>
	class FreeList
	{
		static_assert(isPowerOf2(alignment) || alignment == 0, "alignment parameter must be a power of 2 or 0 if no specific aligmnent is desired.");
		static_assert(blocksize > 0, "blocksize must be greater than 0.");
		static_assert(blocksperchunk > 0, "blocksperchunk must be greater than 0.");

		//! Node data structure for the in-memory list
		typedef struct node
		{
			node* next;
		} node;

		/*!
		\brief Describes a chunk of memory.

		A chunk has a pointer to the allocated memory block, a pointer to the first address in the chunk
		that meets the specified alignment requirement and pointers to the first and the last block.
		*/
		typedef struct chunk
		{
			void* memptr;		//!< Pointer to the underlying memory block
			void* alignedaddr;	//!< First address in the chunk that meets the alignment requirement
			node* head;			//!< Pointer to the first node or block
			node* last;			//!< Pointer to the last node or block
		} chunk;

		//! Current head of the linked list of blocks
		node* head;
		//! Number of chunks allocated in total
		size_t chunk_count;
		//! Blocksize, round up to the next multiple of alignment
		size_t aligned_blocksize;
		//! Size of a chunk, calculated as blocksperchunk * aligned_blocksize
		size_t aligned_chunksize;
		//! All chunks that are currently allocated
		std::vector<chunk> chunks;
		//! Flag that signals that the FreeList is ready to perform allocations and deallocations
		bool initialized;

	private:
		/*!
		\brief Allocates a new chunk and calculates the alignedaddr member.
		*/
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

		/*!
		\brief Splits the chunk into blocks and connects them to an in-memory linked list of free blocks.
		*/
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

		//! Deallocates a chunk
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

		//! Allocates and initializes a new chunk and adds the resulting new, free blocks to the current linked list of free blocks
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
			//increase chunk count
			++chunk_count;
			head = chunks[chunks.size() - 1].head;
			return true;
		}

	public:
		//! Constructor
		FreeList() :
			head(nullptr),
			chunk_count(0),
			aligned_blocksize(0),
			aligned_chunksize(0),
			chunks(),
			initialized(false)
		{
		}

		//! Destructor
		~FreeList()
		{
			for (auto& c : chunks)
			{
				deallocate_chunk(c);
			}
		}

		/*!
		\brief Initializes the FreeList with a single chunk.

		\returns Returns true if the initialization was successful.
		*/
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

		/*!
		\brief Allocates a block of memory.

		Allocates a block of memory with a size up to blocksize and with the alignment specified through the alignment template parameter.
		
		\param[in] size			Size of the memory block to be allocated
		\throw std::bad_alloc	Throws std::bad_alloc if size is larger than the specified block size or if no free block was available and the allocation of a new chunk failed.
		*/
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

		/*!
		\brief Deallocates a block of memory.

		Deallocates a block of memory pointed by ptr.
		\param[in] ptr	Pointer to the block that should be deallocated.

		\attention Never deallocate a block that was allocated on another FreeList instance!
		*/
		void deallocate(void* ptr)
		{
			if (!initialized)
				throw std::logic_error("FreeList allocator is not initialized.");

			node* reclaimed = new(ptr)node;
			reclaimed->next = head;
			head = reclaimed;
		}

		/*!
		\brief Returns true if the given address is from this free list instance.

		\param[in] address	The address to be checked
		\returns			Returns true if the address belongs to a block from this list instance.
		*/
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

	/*!
	\brief Thread safe implementation of a free list allocator

	This class implements a very basic thread safe free list. It is inspired from
	Google's tcmalloc: http://goog-perftools.sourceforge.net/doc/tcmalloc.html

	A central list is used to manage large parts of the free list, it is protected by a mutex.
	Thread-local thread caches are used to cache parts of the linked list for every thread.
	If the local list of blocks is empty on allocation, a new batch of blocks is
	pulled from the central list. If the thread cache exceeds a threshold of free blocks (through deallocations)
	a batch of blocks is pushed back onto the central list. This way the mutex of the central list must only
	be locked every blocksperchunk allocations.

	\attention No instance is created from ThreadSafeFreeList. That means that the whole translation unit shares
	a static instance of the list with the given template parameters. Therefore two rules must be followed:
	1: Do not deallocate a block that was allocated in another translation unit. 2: Do not deallocate a block that
	was allocated by another template instantiation of ThreadSafeFreeList.

	\tparam alignment		Desired alignment of all blocks
	\tparam blocksize		Maximum size of memory blocks that can be allocated
	\tparam blocksperchunk	How many blocks are allocated at once, also determines batch size pulled and pushed onto the central list.
	*/
	template <size_t alignment, size_t blocksize, size_t blocksperchunk>
	class ThreadSafeFreeList
	{
		static_assert(isPowerOf2(alignment) || alignment == 0, "alignment parameter must be a power of 2 or 0 if no specific aligmnent is desired.");
		static_assert(blocksize > 0, "blocksize must be greater than 0.");
		static_assert(blocksperchunk > 0, "blocksperchunk must be greater than 0.");

	private:
		/*!
		\brief Central list of chunks
		*/
		class CentralFreeList
		{
		public:
			using node = struct node
			{
				node* next;
			};

			using batch = struct alignas(TS_CACHE_LINE_SIZE)batch
			{
				node* first;
				//node* last;
				size_t size;
			};
		private:
			typedef struct alignas(TS_CACHE_LINE_SIZE)chunk
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
			/*!
			\brief Allocates a new chunk and calculates the alignedaddr member.
			*/
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

			/*!
			\brief Initializes the free list on the chunks memory range
			*/
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

			/*!
			\brief Deallocates a chunk
			*/
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

			/*!
			\brief Allocates and initilizes a new chunk.
			\returns Returns true on success.
			*/
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

			/*!
			\brief Preallocates n chunks of memory
			\param[in] n Number of chunks to be preallocated.
			*/
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

			/*!
			\brief Initializes the free list.
			\returns Returns true on success.
			*/
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

			/*!
			\brief Pulls a batch of blocks from the central list.
			*/
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

			/*!
			\brief Pushes a batch of blocks back onto the central list.
			*/
			void pushBatch(const batch& b)
			{
				//std::cout << "pullbatch\n";
				std::lock_guard<mutex_t> lock(mtx);
				batches.push_back(b);
			}
		};

		using CList = CentralFreeList;

		/*!
		\brief Caches parts of the free list per thread to reduce contention.

		Allocation and deallocation is done via the thread_local ThreadCache object for
		a specific thread. If no more blocks are available in the cache, a new batch is pulled from the centra list.
		If there are 2 * blocksperchunk free blocks in the cache through many deallocations, a batch of blocksperchunk
		blocks is pushed back onto the central free list.
		*/
		class ThreadCache
		{
		public:
			//ctors/dtor

			//members

			//! Current number of free blocks in the cache
			size_t count; //if count > blocksperchunk save position into split. and update splitend on every incoming free.  if count is then > 2*blocksperchunk push the batch beginning with split back on cl.
			//! Used to split the current free list when a batch is pushed back onto the central list
			size_t splitcount;
			//! Head of the cached free list
			typename CList::node* head;	//if splitlist is not pushed back yet and head is nullptr, head = splitstart and splitstart = splitend = nullptr
			//! Split position
			typename CList::node* split;
			//! Reference to the central list instance
			CList& clst;

			//! Constructor
			ThreadCache(CList& clst) :
				count(0),
				splitcount(0),
				head(nullptr),
				split(nullptr),
				clst(clst)
			{

			}

			//! Destructor. Any free nodes left ware pushed back onto the central list.
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

			/*!
			\brief Allocates a block of memory.

			Allocates blocks of memory up to blocksize bytes. All blocks are aligned as specified by the aligned template parameter.

			\param[in] size		Size of the block to be allocated. Must be > 0 and <= blocksize.
			\returns			Returns a void pointer to the newly allocated block.
			*/
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
			}

			/*!
			\brief Deallocates a block of memory.

			Deallocates a block of memory and internally pushes it onto the cached list of free blocks.
			If the number of free blocks exceeds 2 * blocksperchunk, a batch of blocksperchunk blocks is
			pushed back onto the central list.

			\param[in] ptr	Pointer to the block of memory to be deallocated.
			*/
			void deallocate(void* ptr)
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
			}
		};

		//! Static central list instance
		static CList centralList;
		//! One ThreadCache instance per thread
		static thread_local ThreadCache tc;

	public:
		/*!
		\brief Allocates a block of memory of up to blocksperchunk bytes.

		This function is thread safe. It uses a thread_local instance of ThreadCache.
		\param[in] size		Size of the block to be allocated. Must be > 0 and <= blocksize.
		\returns			Returns a void pointer to the newly allocated block.
		\throws Throws std::bad_alloc if an allocation failed.
		*/
		static inline void* allocate(size_t size)
		{
			if (size > blocksize)
				throw std::bad_alloc();
			return tc.allocate(size);
		}

		/*!
		\brief Deallocates a block of memory.

		This function is thread safe.
		\param[in] ptr	A pointer to the block to be deallocated.

		\attention  Never deallocate a block that was allocated from another template instantiation of ThreadSafeFreeList or in another translation unit.
		*/
		static inline void deallocate(void* ptr)
		{
			tc.deallocate(ptr);
		}

		//! Tells the central list to allocate n chunks in advance.
		static void reservechunks(size_t n)
		{
			centralList.reserve(n);
		}
	};

	//! Definition of the static CentralFreeList instance
	template<size_t alignment, size_t blocksize, size_t blocksperchunk>
	typename ThreadSafeFreeList<alignment, blocksize, blocksperchunk>::CList ThreadSafeFreeList<alignment, blocksize, blocksperchunk>::centralList;

	//! Definition of the thread_local ThreadCache instances
	template<size_t alignment, size_t blocksize, size_t blocksperchunk>
	thread_local typename ThreadSafeFreeList<alignment, blocksize, blocksperchunk>::ThreadCache ThreadSafeFreeList<alignment, blocksize, blocksperchunk>::tc(ThreadSafeFreeList<alignment, blocksize, blocksperchunk>::centralList);

	//---------------------------------------------------------------------------------------------------------------------------------
	

}
#endif
/** @}*/