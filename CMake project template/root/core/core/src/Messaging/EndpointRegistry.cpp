#include <core/messaging/EndpointRegistry.h>

ipengine::EndpointRegistry::EndpointRegistry() :
	m_lock(),
	m_endpoints(),
	m_idgen(1)
{}

ipengine::EndpointRegistry::~EndpointRegistry()
{}

ipengine::EndpointHandle ipengine::EndpointRegistry::createEndpoint(const std::string & name)
{	
	auto ep = EndpointHandle(new MessageEndpoint(m_idgen.fetch_add(1, std::memory_order_relaxed), name));
	std::lock_guard<YieldingSpinLock<5000>> lock(m_lock);
	m_endpoints.insert(std::make_pair(ep->getID(), ep));
	return ep;
}

void ipengine::EndpointRegistry::destroyEndpoint(ipengine::EndpointHandle& handle)
{
	std::lock_guard<YieldingSpinLock<5000>> lock(m_lock);
	if (!handle.isEmpty())
	{
		handle->die();
		m_endpoints.erase(handle->getID());		
	}
}

ipengine::MessageType ipengine::EndpointRegistry::registerMessageType(const ipstring & name)
{
	std::lock_guard<YieldingSpinLock<5000>> lock(m_mtlock);
	auto newid = m_idgen.fetch_add(1, std::memory_order_relaxed);
	m_messageTypes.insert(std::make_pair(newid, name));
	return newid;
}

ipengine::MessageType ipengine::EndpointRegistry::getMessageTypeByName(const ipstring & name)
{
	std::lock_guard<YieldingSpinLock<5000>> lock(m_mtlock);
	auto it = std::find_if(m_messageTypes.begin(), m_messageTypes.end(), [name](std::unordered_map<MessageType, ipengine::ipstring>::value_type& p) { return p.second == name; });
	if (it != m_messageTypes.end())
		return it->first;
	return IPID_INVALID;
}

const ipengine::ipstring & ipengine::EndpointRegistry::getMessageTypeName(MessageType mtype)
{
	std::lock_guard<YieldingSpinLock<5000>> lock(m_mtlock);
	auto& f = m_messageTypes.find(mtype);
	if (f != m_messageTypes.end())
		return f->second;
	return "";
}

ipengine::EndpointHandle ipengine::EndpointRegistry::getEndpoint(ipid id)
{
	std::lock_guard<YieldingSpinLock<5000>> lock(m_lock);
	auto it = m_endpoints.find(id);
	if (it != m_endpoints.end())
	{
		return it->second;
	}
	return EndpointHandle();
}

ipengine::EndpointHandle ipengine::EndpointRegistry::getEndpoint(const std::string & name)
{
	std::lock_guard<YieldingSpinLock<5000>> lock(m_lock);
	auto it = std::find_if(m_endpoints.begin(), m_endpoints.end(), [&name](auto& e) { 
		return e.second->getName() == name; 
	});

	if (it != m_endpoints.end())
	{
		return it->second;
	}

	return EndpointHandle();
}
