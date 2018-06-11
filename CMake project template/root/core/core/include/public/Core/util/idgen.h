#ifndef _ID_GEN_H_
#define _ID_GEN_H_
#include <core/core_types.h>
#include <atomic>

namespace ipengine
{
	class IdGen
	{
	private:
		std::atomic<ipengine::ipid> m_idct;

	public:
		IdGen() :
			m_idct(1)
		{}

		ipengine::ipid createID()
		{
			return m_idct.fetch_add(1, std::memory_order_relaxed);
		}

		void reset()
		{
			m_idct.store(1, std::memory_order_relaxed);
		}
	};
}
#endif