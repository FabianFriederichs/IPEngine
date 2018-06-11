#ifndef _ENDPOINT_REGISTRY_H_
#define _ENDPOINT_REGISTRY_H_
#include <core/core_config.h>
#include <core/messaging/MessageEndpoint.h>
#include <core/util/spinlock.h>
#include <core/core_types.h>
#include <map>

namespace ipengine
{
	using EndpointRecord = std::pair<ipid, MessageEndpoint*>;
	class CORE_API EndpointRegistry
	{
	public:
		EndpointRegistry();
		~EndpointRegistry();

		EndpointHandle createEndpoint(const std::string& name);
		void destroyEndpoint(EndpointHandle& handle);

		//TODO: implement those
		MessageType registerMessageType(const ipstring& name);
		MessageType getMessageTypeByName(const ipstring& name);
		const ipstring& getMessageTypeName(MessageType mtype);

		EndpointHandle getEndpoint(ipid id);
		EndpointHandle getEndpoint(const std::string& name);

	private:
		std::map<ipid, EndpointHandle> m_endpoints;
		std::map<MessageType, ipstring> m_messageTypes;
		YieldingSpinLock<5000> m_lock;
		YieldingSpinLock<5000> m_mtlock;
		std::atomic<ipid> m_idgen;
	};
}

#endif