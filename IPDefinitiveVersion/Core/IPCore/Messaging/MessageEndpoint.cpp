#include "MessageEndpoint.h"

ipengine::MessageEndpoint::MessageEndpoint(ipengine::ipid id, const std::string & name) :
	m_endpointid(id),
	m_name(name)
{

}

ipengine::MessageEndpoint::~MessageEndpoint()
{
	//destroy all messages floating around
	Message* msg;
	while (m_incomingMessages.pop(msg))
	{
		MessageAlloc::deallocate(msg);
	}
	while (m_outgoingMessages.pop(msg))
	{
		MessageAlloc::deallocate(msg);
	}

	//TODO: disconnect all subscribers?
	//possible deadlock hazard
}

bool ipengine::MessageEndpoint::dispatch()
{
	Message* msg = nullptr;
	unsigned int dispatchedMessages = 0;
	while (m_incomingMessages.pop(msg))
	{
		if (msg != nullptr)
		{
			std::unique_lock<YieldingSpinLock<5000>> lock(m_callbacklock);
			auto r = m_callbacks.equal_range(msg->type);
			for (auto it = r.first; it != r.second; ++it)
			{
				it->second(*msg);
				dispatchedMessages++;
			}
			MessageAlloc::deallocate(msg);
		}
	}
	return dispatchedMessages > 0u;
}

bool ipengine::MessageEndpoint::dispatchOne()
{
	Message* msg;
	unsigned int dispatchedMessages = 0;
	if (m_incomingMessages.pop(msg))
	{
		if (msg != nullptr)
		{
			std::unique_lock<YieldingSpinLock<5000>> lock(m_callbacklock);
			auto r = m_callbacks.equal_range(msg->type);
			for (auto it = r.first; it != r.second; ++it)
			{
				it->second(*msg);
				dispatchedMessages++;
			}
			MessageAlloc::deallocate(msg);
		}
	}
	return dispatchedMessages > 0;
}

bool ipengine::MessageEndpoint::connectTo(MessageEndpoint * other)
{
	if (other != nullptr)
	{
		if (other->acceptConnection(this))
		{
			std::unique_lock<YieldingSpinLock<5000>> lock(m_subscriberlock);
			m_subscriptions.push_back(other);
			return true;
		}
	}
	return false;
}

bool ipengine::MessageEndpoint::disconnectFrom(MessageEndpoint * other)
{
	if (other != nullptr)
	{
		if (other->removeConnection(this))
		{
			std::unique_lock<YieldingSpinLock<5000>> lock(m_subscriberlock);
			m_subscriptions.erase(std::remove(m_subscriptions.begin(), m_subscriptions.end(), other), m_subscriptions.end());
			return true;
		}
	}
	return false;
}

void ipengine::MessageEndpoint::sendPendingMessages()
{
	Message* msg = nullptr;
	while (m_outgoingMessages.pop(msg))
	{
		if (msg != nullptr)
		{
			sendMessage(*msg);
		}
	}
}

void ipengine::MessageEndpoint::enqueueMessage(const Message & msg)
{
	Message* m = new(MessageAlloc::allocate(MSIZE))Message(msg);
	m_outgoingMessages.push(m);
}

void ipengine::MessageEndpoint::sendMessage(const Message & msg)
{
	std::unique_lock<YieldingSpinLock<5000>> lock(m_subscriberlock);
	for (auto sub : m_subscribers)
	{
		Message* m = new(MessageAlloc::allocate(MSIZE))Message(msg);
		sub->receiveMessage(m);
	}
}

bool ipengine::MessageEndpoint::registerCallback(const MessageCallback & callback, MessageType type)
{
	//std::unique_lock<std::mutex> lock(m_callbacklock);
	std::unique_lock<YieldingSpinLock<5000>> lock(m_callbacklock);

	if (m_callbacks.count(type) == 0)
	{
		m_callbacks.insert(std::make_pair(type, callback));
		return true;
	}
	return false;
}

bool ipengine::MessageEndpoint::unregisterCallback(MessageType type)
{
	//std::unique_lock<std::mutex> lock(m_callbacklock);
	std::unique_lock<YieldingSpinLock<5000>> lock(m_callbacklock);

	if (m_callbacks.count(type) == 1)
	{
		m_callbacks.erase(type);
		return true;
	}
	return false;
}

const std::string & ipengine::MessageEndpoint::getName() const
{
	return m_name;
}

size_t ipengine::MessageEndpoint::getSubscriberCount()
{
	std::unique_lock<YieldingSpinLock<5000>> lock(m_subscriberlock);
	return m_subscribers.size();
}

size_t ipengine::MessageEndpoint::getSubscriptionCount()
{
	std::unique_lock<YieldingSpinLock<5000>> lock(m_subscriberlock);
	return m_subscriptions.size();
}

ipengine::ipid ipengine::MessageEndpoint::getID() const
{
	return m_endpointid;
}

bool ipengine::MessageEndpoint::acceptConnection(MessageEndpoint * subscriber)
{
	//std::unique_lock<std::mutex> lock(m_subscriberlock);
	std::unique_lock<YieldingSpinLock<5000>> lock(m_subscriberlock);
	if (std::find(m_subscribers.begin(), m_subscribers.end(), subscriber) == m_subscribers.end())
	{
		m_subscribers.push_back(subscriber);
		return true;
	}
	return false;
}

bool ipengine::MessageEndpoint::removeConnection(MessageEndpoint * subscriber)
{
	//std::unique_lock<std::mutex> lock(m_subscriberlock);
	std::unique_lock<YieldingSpinLock<5000>> lock(m_subscriberlock);
	auto it = std::find(m_subscribers.begin(), m_subscribers.end(), subscriber);
	if (it != m_subscribers.end())
	{
		m_subscribers.erase(it);
		return true;
	}
	return false;
}

void ipengine::MessageEndpoint::receiveMessage(Message * msg)
{
	//put the thing into incoming messages queue
	if (msg != nullptr)
	{
		//we could ditch an uninteresting message at this point
		m_incomingMessages.push(msg);
	}
}
