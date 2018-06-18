/** \addtogroup messaging
*  @{
*/

/*!
\file Message.h
\brief Defines the message class
*/
#ifndef _MESSAGE_H_
#define	_MESSAGE_H_
#include <IPCore/RuntimeControl/Time.h>
#include <IPCore/core_config.h>
#include <IPCore/Util/any.h>
#include <IPCore/Core/ICoreTypes.h>

namespace ipengine
{
	/*!
	\brief Types of messages are differentiated through type IDs rather than derived Message types.
	*/
	using MessageType = ipuint32;
	/*!
	\brief The basic datastructure for messages.
	*/
	class Message
	{
	public:	
		/*!
		\brief ID of the sender endpoint.

		The ID of the MessageEndpoint the message was sent from.
		*/
		ipid senderid;
		/*!
		\brief Type ID of the message

		To prevent extensive subclassing for different message types, the type is expressed through
		a simple integer value.
		Type IDs can be associated to type names via the EndpointRegistry.
		*/
		MessageType type;
		/*!
		\brief Point in time the message was sent.
		*/
		ipengine::Time timestamp;
		/*!
		\brief The payload of the message.

		Anything can packed into this member through the use of soo_any at this point.
		A payload size up to 16 bytes is very fast because small object optimization of
		soo_any kicks in. Sending pointers to structs is a good option.
		*/
		ipengine::soo_any payload;
	};
}


#endif
/** @}*/