#ifndef _CORE_CONFIG_H_
#define _CORE_CONFIG_H_
#include <IPCore/Core/ICoreTypes.h>

//general stuff
#ifdef _MSC_VER
	#ifdef EXPORT_IP_CORE
			#define CORE_API __declspec(dllexport)
		#else //
			#define CORE_API __declspec(dllimport)
		#endif
	#else
		#define CORE_API
#endif

//threading services
#define TS_CACHE_LINE_SIZE 128

//messaging
#define MAX_MESSAGE_PAYLOAD_SIZE 256

#endif // !_CORE_CONFIG_H_

