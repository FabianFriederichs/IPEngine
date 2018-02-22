#ifndef _ENDPOINT_REGISTRY_H_
#define _ENDPOINT_REGISTRY_H_
#include <IPCore/core_config.h>
#include <IPCore/Messaging/MessageEndpoint.h>
#include <IPCore/Util/spinlock.h>
#include <IPCore/Core/ICoreTypes.h>
#include <map>

namespace ipengine
{
	using EndpointRecord = std::pair<ipid, MessageEndpoint*>;
	class CORE_API EndpointRegistry
	{
	public:
		EndpointRegistry();
		~EndpointRegistry();

		MessageEndpoint* createEndpoint(const std::string& name);
		void destroyEndpoint(MessageEndpoint*& endpoint);

		//TODO: implement those
		MessageType registerMessageType(const ipstring& name);
		MessageType getMessageTypeByName(const ipstring& name);
		const ipstring& getMessageTypeName(MessageType mtype);

		MessageEndpoint* getEndpoint(ipid id);
		MessageEndpoint* getEndpoint(const std::string& name);

	private:
		std::map<ipid, ipengine::MessageEndpoint*> m_endpoints;
		std::map<MessageType, ipstring> m_messageTypes;
		YieldingSpinLock<5000> m_lock;
		std::atomic<ipid> m_idgen;
	};
}

#endif