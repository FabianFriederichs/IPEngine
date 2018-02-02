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

		MessageEndpoint* getEndpoint(ipid id);
		MessageEndpoint* getEndpoint(const std::string& name);

	private:
		std::map<ipid, ipengine::MessageEndpoint*> m_endpoints;
		YieldingSpinLock<5000> m_lock;
		std::atomic<ipid> m_idgen;
	};
}

#endif