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
#include <IPCore/Util/refctr.h>

#define MT_ALLMESSAGES 0
//TODO: Create some kind of endpoint class. The class should be returned when subscribing to a
//message queue. There should be some functionality for registering callbacks for a specific message type
namespace ipengine
{
	class MessageEndpoint;
	class Message;
	using MessageCallback = ipengine::function<void(Message&)>;
	class EndpointHandle;
	
	class CORE_API MessageEndpoint
	{
		friend class EndpointHandle;
		friend class EndpointRegistry;

	private:
		using MessageAlloc = ipengine::ThreadSafeFreeList<IP_CACHE_LINE_SIZE, sizeof(Message), 4096>;
		static const size_t MSIZE = sizeof(Message);

	public:
		MessageEndpoint(ipid id, const std::string& name);
		~MessageEndpoint();
		bool dispatch();
		bool dispatchOne();
		bool connectTo(EndpointHandle& other);
		bool disconnectFrom(EndpointHandle& other);
		void sendPendingMessages();
		void enqueueMessage(const Message& msg);
		void sendMessage(const Message& msg);
		bool registerCallback(const MessageCallback& callback);
		bool unregisterCallback();
		ipid getID() const;
		const std::string& getName() const;
		size_t getSubscriberCount();	
		bool isDead() const;
	private:
		bool acceptConnection(EndpointHandle& subscriber);
		bool removeConnection(EndpointHandle& subscriber);
		void receiveMessage(Message* msg);
		void die();
		void removeDeadSubscribers();
	private:
		size_t m_endpointid;
		std::string m_name;
		MessageQueue m_incomingMessages;
		MessageQueue m_outgoingMessages;
		std::vector<EndpointHandle> m_subscribers;
		ipengine::YieldingSpinLock<5000> m_subscriberlock;
		MessageCallback m_callback;
		std::atomic<bool> m_dead;
		std::atomic<bool> m_hashandler;
		std::atomic<ipint32> m_subcount;
		RefCtr m_refct;
	};

	class CORE_API EndpointHandle
	{
	private:
		MessageEndpoint* endpoint;
	public:
		EndpointHandle();
		~EndpointHandle();
		EndpointHandle(MessageEndpoint* ep);
		EndpointHandle(const EndpointHandle& other);
		EndpointHandle(EndpointHandle&& other);
		EndpointHandle& operator=(const EndpointHandle& other);
		EndpointHandle& operator=(EndpointHandle&& other);
		MessageEndpoint* operator->();
		bool isEmpty() const;
		bool operator==(const EndpointHandle& other) const;
		bool operator!=(const EndpointHandle& other) const;
	};
}


#endif
