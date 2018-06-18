/** \addtogroup messaging
*  @{
*/

/*!
\file MessageQueue.h
*/
#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_
#include <IPCore/libs/concurrentqueue.h>
#include <IPCore/Messaging/Message.h>
#include <IPCore/core_config.h>

namespace ipengine
{
	/*!
	\brief A very basic message queue wrapper.

	This message queue uses the implementation of a lockfree concurrent queue from "Cameron":
	https://github.com/cameron314/concurrentqueue

	\todo Another, more lightweight queue implementation should be implemented. The moodycamel queue is
	very fast and scales nicely but is unbounded. For a messaging system a bounded-type queue is more
	appropriate, because it prevents memory overflows if consumers are slower than producers.
	*/
	class CORE_API MessageQueue
	{
	public:
		MessageQueue();
		~MessageQueue();
		//! Push a message
		bool push(Message* msg);
		//! Pull a message
		bool pop(Message *&msg);

	private:
		moodycamel::ConcurrentQueue<Message*, moodycamel::ConcurrentQueueDefaultTraits> m_queue;
	};
}


#endif
/** @}*/