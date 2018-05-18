#ifndef _CORE_HOST_H_
#define _CORE_HOST_H_

template <typename ... CoreModules>
class Core : public CoreModules...
{
public:
	Core(CoreModules&&... coreModules) :

};

#endif