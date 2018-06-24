/** \addtogroup messaging
Implements messaging functionality of the core.
*  @{
*/

/*!
\file MessageEndpoint.h
*/
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

namespace ipengine
{
	class MessageEndpoint;
	class Message;
	/*!
	\brief Callback type for message handlers
	*/
	using MessageCallback = ipengine::function<void(Message&)>;
	class EndpointHandle;

	/*!
	\brief Implements the main functionality of the messaging system.

	An endpoint has an incoming and an outgoing message queue. Message that should be sent are pushed
	onto the outgoing queue, received meshes are enqueued in the incoming queue.
	registerCallback registers a callback function for receiving messages.
	The dispatch and dispatchOne functions then pop one or many messages from the incoming queue and
	pass them to the registered callback.
	enqueueMessage enqueues a message in the outgoing queue, sendPendingMessages sends all enqueued messages at once.
	sendMessage immediately sends a single message without buffering in the queue.

	The functions connectTo and disconnectFrom are used to manage connections to other endpoints.
	When a message is sent, it is pushed onto the incoming queues of all connected subscribers.
	*/
	class CORE_API MessageEndpoint
	{
		friend class EndpointHandle;
		friend class EndpointRegistry;

	private:
		/*!
		\brief Messages are allocated via the ThreadSafeFreeList allocator.
		*/
		using MessageAlloc = ipengine::ThreadSafeFreeList<TS_CACHE_LINE_SIZE, sizeof(Message), 4096>;
		//! Size of a single message.
		static const size_t MSIZE = sizeof(Message);

	public:
		//! Initializes an endpoint with an ID and a name.
		MessageEndpoint(ipid id, const std::string& name);
		//! Destructor
		~MessageEndpoint();
		/*!
		\brief Calls the registered message callback for all available messages on the incoming queue.
		\returns Returns true if any messages were dispatched.
		*/
		bool dispatch();
		/*!
		\brief Calls the registered message callback for one message from the incoming queue.
		\returns Returns true if a message was dispatched.
		*/
		bool dispatchOne();
		/*!
		\brief Connects this endpoint to another endpoint.

		If the connection is established successfully, messages sent from the other endpoint are pushed onto
		the incoming queue of this object.
		\returns Returns true if the connection was established successfully.
		*/
		bool connectTo(EndpointHandle& other);
		/*!
		\brief Disconnects from another endpoint.
		\returns	Returns true if a connection was successfully removed. If there was no connection, false is returned.
		*/
		bool disconnectFrom(EndpointHandle& other);
		/*!
		\brief Pushes all messages from the outgoing queue onto the incoming queues of all subscribers i.e. connected endpoints.

		For many messages this is more efficient than sending every message immediately with sendMessage.
		*/
		void sendPendingMessages();
		/*!
		\brief Enqueues a message to be sent.

		Enqueues the given message.
		\param[in] msg	The message to enqueue
		*/
		void enqueueMessage(const Message& msg);
		/*!
		\brief Immediately sends a message.

		Immediately sends the given message.
		\param[in] msg	The message to send
		*/
		void sendMessage(const Message& msg);
		/*!
		\brief Registers a callback for handling the messages.

		The callback is called for every incoming message that is dispatched.
		If the callback is invalid or empty, incoming messages are ignored
		to prevent a memory leak.
		If a callback is present already, it is replaced by the new one.
		
		\param[in] callback	The callback to be registered.
		\returns Returns true.
		*/
		bool registerCallback(const MessageCallback& callback);
		/*!
		\brief Unregisters any registered callback.
		\returns Returns true.
		*/
		bool unregisterCallback();
		/*!
		\brief Returns the endpoint's ID.
		\return Returns the endpoint's ID.
		*/
		ipid getID() const;
		/*!
		\brief Returns the endpoint's name.
		\return Returns the endpoint's name.
		*/
		const std::string& getName() const;
		/*!
		\brief Returns the number of subscribers.

		Returns the number of endpoints that are connected (via connectTo) to this endpoint,
		i.e. the number of receivers of messages that are sent from this endpoint.

		\returns Returns the number of connected subscribers.
		*/
		size_t getSubscriberCount();	
		/*!
		\brief Returns true if the endpoint is about to be closed down.
		\returns Returns true if the endpoint is about to be closed.
		*/
		bool isDead() const;
	private:
		//! Accept a connection request. Returns true if accepted.
		bool acceptConnection(EndpointHandle& subscriber);
		//! Try to remove a connection. Returns true if there was a conenction to be removed.
		bool removeConnection(EndpointHandle& subscriber);
		//! Is called to enqueue a message in the incoming queue of this endpoint.
		void receiveMessage(Message* msg);
		//! Mark the endpoint as closing-down.
		void die();
		//! Is called repeatedly to remove dead endpoints from the subscriber list.
		void removeDeadSubscribers();
	private:
		size_t m_endpointid; //!< The ID of this endpoint.
		std::string m_name; //!< The name of this endpoint.
		MessageQueue m_incomingMessages; //!< Incoming message queue.
		MessageQueue m_outgoingMessages; //!< Outgoing message queue.
		std::vector<EndpointHandle> m_subscribers; //!< List of subscribers.
		ipengine::YieldingSpinLock<5000> m_subscriberlock; //!< Lock to secure access to subscriber list.
		MessageCallback m_callback; //!< The callback for handling messages.
		std::atomic<bool> m_dead; //!< Indicates that the endpoint is closed down.
		std::atomic<bool> m_hashandler; //!< True if the endpoint has any handler callback registered.
		std::atomic<ipint32> m_subcount; //!< Current number of subscribers.
		RefCtr m_refct; //!< A reference count that is used by EndpointHandle.
	};

	/*!
	\brief A reference counted handle to manage lifetime of MessageEndpoint objects.

	Provides access to all public members of MessageEndpoint via the -> operator.
	Is copy- and move constructible and assignable. isEmpty tells if the handle is valid.
	When the last EndpointHandle is destroyed, the handled MessageEndpoint is destroyed.
	*/
	class CORE_API EndpointHandle
	{
	private:
		//! Pointer to the handled endpoint.
		MessageEndpoint* endpoint;
	public:
		EndpointHandle();
		~EndpointHandle();
		EndpointHandle(MessageEndpoint* ep);
		EndpointHandle(const EndpointHandle& other);
		EndpointHandle(EndpointHandle&& other);
		EndpointHandle& operator=(const EndpointHandle& other);
		EndpointHandle& operator=(EndpointHandle&& other);
		/*!
		\brief Allows to access all public MessageEndpoint members.
		*/
		MessageEndpoint* operator->();
		/*!
		\brief Returns true if the handle is valid.
		\returns Returns true if the handle is valid.
		*/
		bool isEmpty() const;
		/*!
		\brief Returns true if this and other are handles for the same MessageEndpoint.
		*/
		bool operator==(const EndpointHandle& other) const;
		/*!
		\brief Returns true if this and other are handles for different MessageEndpoints.
		*/
		bool operator!=(const EndpointHandle& other) const;
	};
}


#endif
/** @}*/