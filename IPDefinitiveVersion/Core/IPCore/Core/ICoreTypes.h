#ifndef _I_CORE_TYPES_
#define _I_CORE_TYPES_
#include <cstdint>
#include <string>
#include <IPCore/core_config.h>
namespace ipengine
{
	//identifiers
#define IPID_INVALID 0
	using ipid = uint64_t;
	using ipsize = size_t;
	//primitive types
	using ipuint8 = uint8_t;
	using ipuint16 = uint16_t;
	using ipuint32 = uint32_t;
	using ipuint64 = uint64_t;
	
	using ipint8 = int8_t;
	using ipint16 = int16_t;
	using ipint32 = int32_t;
	using ipint64 = int64_t;

	using ipfloat = float;
	using ipdouble = double;
	using ipbool = bool;

	//some other stuff
	using iprstr = char*;
	using ipcrstr = const char*;
	using ipchar = char;

	//export string from here	
	/*CORE_API_TEMPLATE template CORE_API class std::basic_string<ipchar, std::char_traits<ipchar>, std::allocator<ipchar>>;*/
	using ipstring = std::basic_string<ipchar, std::char_traits<ipchar>, std::allocator<ipchar>>;
	
}

#endif
