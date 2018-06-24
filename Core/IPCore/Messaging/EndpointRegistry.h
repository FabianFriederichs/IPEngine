/** \addtogroup messaging
*  @{
*/

/*!
\file EndpointRegistry.h
*/

#ifndef _ENDPOINT_REGISTRY_H_
#define _ENDPOINT_REGISTRY_H_
#include <IPCore/core_config.h>
#include <IPCore/Messaging/MessageEndpoint.h>
#include <IPCore/Util/spinlock.h>
#include <IPCore/Core/ICoreTypes.h>
#include <map>

namespace ipengine
{
	//! Convenience typedef
	using EndpointRecord = std::pair<ipid, MessageEndpoint*>;
	/*!
	\brief Publicly visible list of MessageEndpoint objects and MessageType definitions.

	This class is used to create MessageEndpoint objects that are publicly visible to
	other modules and accessible by name or id.
	Beside that, the class provides a mapping between names and integral identifiers
	for message types.
	*/
	class CORE_API EndpointRegistry
	{
	public:
		//! Constructor
		EndpointRegistry();
		//! Destructor
		~EndpointRegistry();

		/*!
		\brief Creates an endpoint with the given name and returns a reference counted handle to that endpoint.

		The function creates a new MessageEndpoint with the given name and returns a handle that allows
		access to it.

		\param[in] name		Name of the new MessageEndpoint being created
		*/
		EndpointHandle createEndpoint(const std::string& name);
		/*!
		\brief Marks the endpoint handled by handle as dead and invalidates the handle.

		When an endpoint is destroyed, it is marked as dead. Other endpoints that have handles
		to the now marked endpoint (due to a subscription) will eventually destroy their handles.
		If every handle to the endpoint is destroyed, the actual endpoint will be destroyed, too.

		\param[in] handle		A handle to the endpoint that should be destroyed.

		\todo	Make sure that only the owner of the endpoint can destroy it.
		*/
		void destroyEndpoint(EndpointHandle& handle);

		/*!
		\brief Creates a new message type with the given name.

		\param[in] name		The name of the new message type that should be created.
		\returns			The new MessageType or IPID_INVALID if the name already exists.
		*/
		MessageType registerMessageType(const ipstring& name);
		/*!
		\brief Returns the MessageType with the given name.

		\param[in] name		The name of the message type.
		\returns	Returns the MessageType with the given name or IPID_INVALID if that message type doesn't exist.
		*/
		MessageType getMessageTypeByName(const ipstring& name);

		/*!
		\brief Returns the name of a given MessageType.

		\param[in] mtype	The message type to query the name for
		\returns	Returns the name of a given message type or an empty string if the message type doesn't exist.
		*/
		const ipstring& getMessageTypeName(MessageType mtype);

		/*!
		\brief Returns an EndpointHandle to the MessageEndpoint with the given id.

		\param[in] id	The message endpoint id to query a handle for
		\returns	Returns the EndpointHandle or an empty EndpointHandle if the MessageEndpoint doesn't exist.
		*/
		EndpointHandle getEndpoint(ipid id);

		/*!
		\brief Returns an EndpointHandle to the MessageEndpoint with the given name.

		\param[in] name	The message endpoint name to query a handle for
		\returns	Returns the EndpointHandle or an empty EndpointHandle if the MessageEndpoint doesn't exist.
		*/
		EndpointHandle getEndpoint(const std::string& name);

	private:
		std::map<ipid, EndpointHandle> m_endpoints; //!< Maps endpoint ids to endpoint handles
		std::map<MessageType, ipstring> m_messageTypes; //!< Maps message types to their names
		YieldingSpinLock<5000> m_lock; //!< To secure access to the endpoint map
		YieldingSpinLock<5000> m_mtlock; //!< To secure access to the message type map
		std::atomic<ipid> m_idgen;	//!< Used to generate endpoint ids and message ids
									//!< \todo Use the central IdGen instance in the core
	};
}

#endif

/** @}*/