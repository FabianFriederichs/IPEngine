/** \addtogroup core
*  @{
*/

/*!
\file core_config.h
\brief This file defines symbol import/export for shared libraries on windows platforms
*/

#ifndef _CORE_CONFIG_H_
#define _CORE_CONFIG_H_

#ifdef _MSC_VER
	//! Define EXPORT_IP_CORE if the core should be built.
	#ifdef EXPORT_IP_CORE
			//! Use CORE_API as type modifier for core components that must be exported.
			#define CORE_API __declspec(dllexport)
		#else
			//! Use CORE_API as type modifier for core components that must be exported
			#define CORE_API __declspec(dllimport)
		#endif
	#else
		//! Use CORE_API as type modifier for core components that must be exported
		#define CORE_API
#endif

//! Defines cache line size. This defintion should be moved into the PAL once it's ready.
#define TS_CACHE_LINE_SIZE 128

#endif
/** @}*/