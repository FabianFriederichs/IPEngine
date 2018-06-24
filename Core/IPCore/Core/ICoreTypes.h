/** \addtogroup typelibrary
*  @{
*/

/*!
\file ICoreTypes.h
\brief Contains common type definitions for the whole system.
*/

#ifndef _I_CORE_TYPES_
#define _I_CORE_TYPES_
#include <cstdint>
#include <string>
#include <IPCore/core_config.h>
namespace ipengine
{
	//identifiers
//! 0 is always an invalid identifier
#define IPID_INVALID 0
	//! identifier type is defined as an 64-bit unsigned integer
	using ipid = uint64_t;
	//! type that is large engough to hold any array index on the given platform
	using ipsize = size_t;
	//primitive types
	//! unsigned 8-bit integer
	using ipuint8 = uint8_t;
	//! unsigned 16-bit integer
	using ipuint16 = uint16_t;
	//! unsigned 32-bit integer
	using ipuint32 = uint32_t;
	//! unsigned 64-bit integer
	using ipuint64 = uint64_t;
	
	//! signed 8-bit integer
	using ipint8 = int8_t;
	//! signed 16-bit integer
	using ipint16 = int16_t;
	//! signed 32-bit integer
	using ipint32 = int32_t;
	//! signed 64-bit integer
	using ipint64 = int64_t;

	//! single precision floating point type
	using ipfloat = float;
	//! double precision floating point type
	using ipdouble = double;
	//! boolean type
	using ipbool = bool;

	//some other stuff
	//! pointer-to-char
	using iprstr = char*;
	//! pointer-to-const-char
	using ipcrstr = const char*;
	//! char
	using ipchar = char;

	//! basid std::string (replace with custom string implementation when ready!)
	using ipstring = std::basic_string<ipchar, std::char_traits<ipchar>, std::allocator<ipchar>>;
}

#endif
/** @}*/