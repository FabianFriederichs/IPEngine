#ifndef _MESSAGE_ENDPOINT_H_
#define _MESSAGE_ENDPOINT_H_
#include <IPCore/Messaging/MessageQueue.h>
#include <IPCore/Messaging/Message.h>
#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <IPCore/Util/function.h>
#include <IPCore/Memory/lowlevel_allocators.h>

#define MT_ALLMESSAGES 0
//TODO: Create some kind of endpoint class. The class should be returned when subscribing to a
//message queue. There should be some functionality for registering callbacks for a specific message type
namespace ipengine
{
	class Message;
	using MessageCallback = ipengine::function<void(Message&)>;
	class CORE_API MessageEndpoint
	{
	private:
		using MessageAlloc = ipengine::ThreadSafeFreeList<TS_CACHE_LINE_SIZE, sizeof(Message), 4096>;
		static const size_t MSIZE = sizeof(Message);

	public:
		MessageEndpoint(ipid id, const std::string& name);
		~MessageEndpoint();
		bool dispatch();
		bool dispatchOne();
		bool connectTo(MessageEndpoint* other);
		bool disconnectFrom(MessageEndpoint* other);
		void sendPendingMessages();
		void enqueueMessage(const Message& msg);
		void sendMessage(const Message& msg);
		bool registerCallback(const MessageCallback& callback, MessageType type);
		bool unregisterCallback(MessageType type);
		ipid getID() const;
		const std::string& getName() const;
		size_t getSubscriberCount();
		size_t getSubscriptionCount();
	private:
		bool acceptConnection(MessageEndpoint* subscriber);
		bool removeConnection(MessageEndpoint* subscriber);
		void receiveMessage(Message* msg);
	private:
		size_t m_endpointid;
		std::string m_name;
		MessageQueue m_incomingMessages;
		MessageQueue m_outgoingMessages;
		std::vector<MessageEndpoint*> m_subscribers;
		std::vector<MessageEndpoint*> m_subscriptions;
		ipengine::YieldingSpinLock<5000> m_subscriberlock;
		ipengine::YieldingSpinLock<5000> m_callbacklock;
		std::unordered_multimap<MessageType, MessageCallback> m_callbacks;
	};
}


#endif
