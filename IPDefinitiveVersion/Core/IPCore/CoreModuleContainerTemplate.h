
#include <IPCore/core_config.h>

//put datastructures needed for your interface in a seperate header and include that here

#ifndef _CORE_MODULE_MODULENAME_H_
#define _CORE_MODULE_MODULENAME_H_

//define datastructures needed for your interfaces

class CORE_API CM_MODULENAME
{
public:
	CM_MODULENAME(/*add parameters needed to initialize the implementation*/);
	CM_MODULENAME(const CM_MODULENAME&) = delete;
	CM_MODULENAME(CM_MODULENAME&&) = default;
	CM_MODULENAME& operator=(const CM_MODULENAME&) = delete;
	CM_MODULENAME& operator=(CM_MODULENAME&&) = delete;

	//add methods which should be publicly accessible by the core clients
private:
	class MODULENAMEImpl;
	MODULENAMEImpl* MODULENAME_impl;
};

#endif