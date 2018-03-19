#include "MessageEndpoint.h"

ipengine::MessageEndpoint::MessageEndpoint(ipengine::ipid id, const std::string & name) :
	m_endpointid(id),
	m_name(name),
	m_subcount(0),
	m_hashandler(false)
{

}

ipengine::MessageEndpoint::~MessageEndpoint()
{
	//destroy all messages floating around
	Message* msg;
	while (m_incomingMessages.pop(msg))
	{
		if(msg)
			MessageAlloc::deallocate(msg);
	}
	while (m_outgoingMessages.pop(msg))
	{
		if(msg)
			MessageAlloc::deallocate(msg);
	}
}

bool ipengine::MessageEndpoint::dispatch()
{
	if (m_hashandler.load(std::memory_order_acquire))
	{
		Message* msg = nullptr;
		unsigned int dispatchedMessages = 0;
		while (m_incomingMessages.pop(msg))
		{
			if (msg != nullptr)
			{
				if (!m_callback.isEmpty())
					m_callback(*msg);
				dispatchedMessages++;
				MessageAlloc::deallocate(msg);
			}
		}
		return dispatchedMessages > 0u;
	}
	return false;
}

bool ipengine::MessageEndpoint::dispatchOne()
{
	if (m_hashandler.load(std::memory_order_acquire))
	{
		Message* msg;
		unsigned int dispatchedMessages = 0;
		if (m_incomingMessages.pop(msg))
		{
			if (msg != nullptr)
			{	
				if(!m_callback.isEmpty())
					m_callback(*msg);
				dispatchedMessages++;			
				MessageAlloc::deallocate(msg);
			}
		}
		return dispatchedMessages > 0;
	}
	return false;
}

bool ipengine::MessageEndpoint::connectTo(EndpointHandle& other)
{
	if (!other.isEmpty() && this->getID() != other->getID())
	{
		if (other->acceptConnection(EndpointHandle(this)))
		{
			return true;
		}
	}
	return false;
}

bool ipengine::MessageEndpoint::disconnectFrom(EndpointHandle& other)
{
	if (!other.isEmpty() && getID() != other->getID())
	{
		if (other->removeConnection(EndpointHandle(this)))
		{
			return true;
		}
	}
	return false;
}

void ipengine::MessageEndpoint::sendPendingMessages()
{
	if (m_subcount.load(std::memory_order_relaxed) > 0)
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
}

void ipengine::MessageEndpoint::enqueueMessage(const Message & msg)
{
	if (m_subcount.load(std::memory_order_relaxed) > 0)
	{
		Message* m = new(MessageAlloc::allocate(MSIZE))Message(msg);
		m_outgoingMessages.push(m);
	}
}

void ipengine::MessageEndpoint::sendMessage(const Message & msg)
{
	if (m_subcount.load(std::memory_order_relaxed) > 0)
	{
		std::unique_lock<YieldingSpinLock<5000>> lock(m_subscriberlock);
		bool founddeadendpoints = false;
		for (auto& sub : m_subscribers)
		{
			if (sub->m_dead.load(std::memory_order_relaxed))
			{
				founddeadendpoints = true;
				continue;
			}
			Message* m = new(MessageAlloc::allocate(MSIZE))Message(msg);
			sub->receiveMessage(m);
		}
		lock.unlock();
		if (founddeadendpoints)
			removeDeadSubscribers();
	}
}

bool ipengine::MessageEndpoint::registerCallback(const MessageCallback & callback)
{
	m_callback = callback;
	m_hashandler.store(true, std::memory_order_release);
	return true;
}

bool ipengine::MessageEndpoint::unregisterCallback()
{
	m_callback = MessageCallback();
	m_hashandler.store(false, std::memory_order_release);
	return true;
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

bool ipengine::MessageEndpoint::isDead() const
{
	return m_dead.load(std::memory_order_relaxed);
}

ipengine::ipid ipengine::MessageEndpoint::getID() const
{
	return m_endpointid;
}

bool ipengine::MessageEndpoint::acceptConnection(EndpointHandle& subscriber)
{
	//std::unique_lock<std::mutex> lock(m_subscriberlock);
	std::unique_lock<YieldingSpinLock<5000>> lock(m_subscriberlock);
	if (std::find(m_subscribers.begin(), m_subscribers.end(), subscriber) == m_subscribers.end())
	{
		m_subscribers.push_back(subscriber);
		m_subcount.fetch_add(1, std::memory_order_relaxed);
		return true;
	}
	return false;
}

bool ipengine::MessageEndpoint::removeConnection(EndpointHandle& subscriber)
{
	//std::unique_lock<std::mutex> lock(m_subscriberlock);
	std::unique_lock<YieldingSpinLock<5000>> lock(m_subscriberlock);
	if (std::find(m_subscribers.begin(), m_subscribers.end(), subscriber) != m_subscribers.end())
	{
		m_subscribers.erase(std::remove(m_subscribers.begin(), m_subscribers.end(), subscriber), m_subscribers.end());
		m_subcount.fetch_sub(1, std::memory_order_relaxed);
		return true;
	}
	return false;
}

void ipengine::MessageEndpoint::receiveMessage(Message * msg)
{
	//put the thing into incoming messages queue
	if (m_hashandler.load(std::memory_order_relaxed) && msg != nullptr)
	{
		//we could ditch an uninteresting message at this point
		m_incomingMessages.push(msg);
	}
}

void ipengine::MessageEndpoint::die()
{
	m_dead.store(true, std::memory_order_relaxed);
}

void ipengine::MessageEndpoint::removeDeadSubscribers()
{
	std::unique_lock<YieldingSpinLock<5000>> lock(m_subscriberlock);
	m_subscribers.erase(std::remove_if(m_subscribers.begin(), m_subscribers.end(), [](EndpointHandle& h) { return h->isDead(); }), m_subscribers.end());
}

ipengine::EndpointHandle::EndpointHandle() :
	endpoint(nullptr)
{

}

ipengine::EndpointHandle::~EndpointHandle()
{
	if (endpoint && endpoint->m_refct.dec())
	{
		delete endpoint;
	}		
}

ipengine::EndpointHandle::EndpointHandle(MessageEndpoint * ep) :
	endpoint(ep)
{
	assert(ep);
	endpoint->m_refct.inc();
}

ipengine::EndpointHandle::EndpointHandle(const EndpointHandle & other) :
	endpoint(other.endpoint)
{
	if(endpoint)
		endpoint->m_refct.inc();
}

ipengine::EndpointHandle::EndpointHandle(EndpointHandle && other) :
	endpoint(other.endpoint)
{
	other.endpoint = nullptr;
}

ipengine::EndpointHandle & ipengine::EndpointHandle::operator=(const EndpointHandle & other)
{
	if (this == &other)
		return *this;

	if (endpoint)
	{
		if (endpoint->m_refct.dec())
			delete endpoint;
	}

	endpoint = other.endpoint;
	if(endpoint)
		endpoint->m_refct.inc();

	return *this;
}

ipengine::EndpointHandle & ipengine::EndpointHandle::operator=(EndpointHandle && other)
{
	if (this == &other)
		return *this;
	
	if (endpoint)
	{
		if (endpoint->m_refct.dec())
			delete endpoint;
	}

	endpoint = other.endpoint;
	other.endpoint = nullptr;

	return *this;
}

ipengine::MessageEndpoint * ipengine::EndpointHandle::operator->()
{
	return endpoint;
}

bool ipengine::EndpointHandle::isEmpty() const
{
	return endpoint == nullptr;
}

bool ipengine::EndpointHandle::operator==(const EndpointHandle & other) const
{
	return (endpoint == nullptr && other.endpoint == nullptr) || (endpoint->getID() == other.endpoint->getID());
}

bool ipengine::EndpointHandle::operator!=(const EndpointHandle & other) const
{
	return !(*this == other);
}
