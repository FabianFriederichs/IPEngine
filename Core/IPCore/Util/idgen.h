/** \addtogroup util
*  @{
*/

/*!
\file idgen.h
*/
#ifndef _ID_GEN_H_
#define _ID_GEN_H_
#include <IPCore/Core/ICoreTypes.h>
#include <atomic>

namespace ipengine
{
	/*!
	\brief A simple ID generator based on an atomic counter.

	0 is always an invalid ID.
	*/
	class IdGen
	{
	private:
		std::atomic<ipengine::ipid> m_idct;

	public:
		IdGen() :
			m_idct(1)
		{}

		/*!
		\brief Returns a new ID that is unique as long as the generator is not reset.
		
		This function is thread safe and lock-free.
		*/
		ipengine::ipid createID()
		{
			return m_idct.fetch_add(1, std::memory_order_relaxed);
		}

		/*!
		\brief Resets the atomic counter.
		\attention	Don't call this function if you don't own the generator and know what you're doing.
		\attention	Breaks uniqueness between IDs if any IDs that were generated before the reset are still used.
		*/
		void reset()
		{
			m_idct.store(1, std::memory_order_relaxed);
		}
	};
}
#endif
/** @}*/