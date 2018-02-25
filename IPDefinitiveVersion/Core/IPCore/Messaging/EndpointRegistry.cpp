#include "EndpointRegistry.h"

ipengine::EndpointRegistry::EndpointRegistry() :
	m_lock(),
	m_endpoints(),
	m_idgen(1)
{}

ipengine::EndpointRegistry::~EndpointRegistry()
{}

ipengine::MessageEndpoint * ipengine::EndpointRegistry::createEndpoint(const std::string & name)
{
	MessageEndpoint* ep = new MessageEndpoint(m_idgen.fetch_add(1, std::memory_order_relaxed), name);
	m_endpoints.insert(std::make_pair(ep->getID(), ep));
	return ep;
}

void ipengine::EndpointRegistry::destroyEndpoint(MessageEndpoint *& endpoint)
{
	if (endpoint != nullptr && endpoint->getSubscriberCount() == 0 && endpoint->getSubscriptionCount() == 0)
	{
		delete endpoint;
		endpoint = nullptr;
	}
}

ipengine::MessageType ipengine::EndpointRegistry::registerMessageType(const ipstring & name)
{
	return MessageType();
}

ipengine::MessageType ipengine::EndpointRegistry::getMessageTypeByName(const ipstring & name)
{
	return MessageType();
}

const ipengine::ipstring & ipengine::EndpointRegistry::getMessageTypeName(MessageType mtype)
{
	// TODO: hier Rückgabeanweisung eingeben
	return IPID_INVALID;
}

ipengine::MessageEndpoint * ipengine::EndpointRegistry::getEndpoint(ipid id)
{
	auto it = m_endpoints.find(id);
	if (it != m_endpoints.end())
	{
		return it->second;
	}
	return nullptr;
}

ipengine::MessageEndpoint * ipengine::EndpointRegistry::getEndpoint(const std::string & name)
{
	auto it = std::find_if(m_endpoints.begin(), m_endpoints.end(), [&name](const auto& e) { 
		return e.second->getName() == name; 
	});

	if (it != m_endpoints.end())
	{
		return it->second;
	}

	return nullptr;
}
