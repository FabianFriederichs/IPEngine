#ifndef _CORE_CONFIG_H_
#define _CORE_CONFIG_H_

#ifdef EXPORT_IP_CORE
	#ifdef _MSC_VER
		#define CORE_API __declspec(dllexport)
	#else
		#define CORE_API
	#endif
	#define CORE_API_TEMPLATE
#else
	#ifdef _MSC_VER
		#define CORE_API __declspec(dllimport)
	#else
		#define CORE_API
	#endif
	#define CORE_API_TEMPLATE extern
#endif

//threading services (TODO: put constants into another header)
#define IP_CACHE_LINE_SIZE 128

#endif // !_CORE_CONFIG_H_

