#ifndef _MESSAGE_H_
#define	_MESSAGE_H_
#include <IPCore/RuntimeControl/Time.h>
#include <IPCore/core_config.h>
#include <IPCore/Util/any.h>

//TODO: Create some class "Message". Possible attributes of such a thing:
//		Type
//		ID
//		Payload (maybe in form of an any? or just plain bytes?)
//		Timestamp
//		Sender
//		Receiver(?)

//Question:
//Single signature for handling all messages or a type definition template thingy somewhere else?

namespace ipengine
{
	using MessageType = unsigned int;
	class Message
	{
	public:
		//data		
		ipid senderid;
		MessageType type;
		ipengine::Time timestamp;
		ipengine::soo_any payload;
	};
}


#endif
