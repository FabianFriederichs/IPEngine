#include <core/messaging/MessageQueue.h>

inline ipengine::MessageQueue::MessageQueue() :
	m_queue(512)
{

}

inline ipengine::MessageQueue::~MessageQueue()
{

}

inline bool ipengine::MessageQueue::push(Message * msg)
{
	return m_queue.enqueue(msg);
}

inline bool ipengine::MessageQueue::pop(Message *& msg)
{
	return m_queue.try_dequeue(msg);
}
