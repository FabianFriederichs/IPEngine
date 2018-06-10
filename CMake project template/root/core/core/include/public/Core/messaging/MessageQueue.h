#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_
#include <IPCore/libs/concurrentqueue.h>
#include <IPCore/Messaging/Message.h>
#include <IPCore/core_config.h>

namespace ipengine
{
	class CORE_API MessageQueue
	{
	public:
		MessageQueue();
		~MessageQueue();
		bool push(Message* msg);
		bool pop(Message *&msg);

	private:
		moodycamel::ConcurrentQueue<Message*, moodycamel::ConcurrentQueueDefaultTraits> m_queue;
	};
}


#endif
