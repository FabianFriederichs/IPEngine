/** \addtogroup util
Implements some utility functionality.
*  @{
*/

/*!
\file handle.h
*/
#ifndef _HANDLE_H_
#define _HANDLE_H_

/*!
\brief	Not implemented yet.
\todo	Implement a template for reference counting handles.
*/
template <typename T>
class Handle
{
private:
	T* handledObject;
};

#endif
/** @}*/