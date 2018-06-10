#pragma once
#include <atomic>
#include <thread>
#include <stdexcept>


//----------- README -----------
// Acquire a hazard pointer in your code as follows:
//
//	hptr& hazptr = get_hp_for_current_thread();
//	
//	...
//	
//	node* tmp;
//	do //loop to make sure that the element pointed by oldhead wasn't deleted between reading of head and writing the hp.
//	{
//		tmp = oldhead;
//		hazptr.store(oldhead);
//		oldhead = head.load();
//	} while (oldhead != tmp);
//	//from this point, no thread can delete the node
//	

//the hazard pointer type that is intended for use in code
#define MAX_HAZARD_POINTERS 100

namespace ipengine {
	using hptr = std::atomic<void*>;


	typedef struct hazard_pointer
	{
		std::atomic<std::thread::id> id;
		std::atomic<void*> pointer;
	} hazard_pointer;

	hazard_pointer hzptrs[MAX_HAZARD_POINTERS];

	class hp_owner
	{
		hazard_pointer* hp;

	public:
		//delete some constructor types!
		hp_owner(const hp_owner&) = delete;
		hp_owner& operator=(const hp_owner&) = delete;

		hp_owner() :
			hp(nullptr)
		{
			for (int i = 0; i < MAX_HAZARD_POINTERS; i++)
			{
				std::thread::id old_id;
				if (hzptrs[i].id.compare_exchange_strong(old_id, std::this_thread::get_id()))
				{
					hp = &hzptrs[i];
					break;
				}
			}
			if (!hp)
			{
				throw std::runtime_error("No hazard pointer available.");
			}
		}

		~hp_owner()
		{
			hp->pointer.store(nullptr);
			hp->id.store(std::thread::id());
		}

		hptr& get_ptr()
		{
			return hp->pointer;
		}
	};

	//returns the hazard pointer for this thread
	inline hptr& get_hp_for_current_thread()
	{
		thread_local static hp_owner h;
		return h.get_ptr();
	}

	//returns true if there is a hazard pointer that references ptr
	bool outstanding_hp_for(void* ptr)
	{
		for (int i = 0; i < MAX_HAZARD_POINTERS; i++)
		{
			if (hzptrs[i].pointer.load() == ptr)
				return true;
		}
		return false;
	}

}