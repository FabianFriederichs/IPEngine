#include "IModule_API.h"
#include "Core/ICore.h"
class IModulCoreTest_API : public IModule_API
{
public:
	virtual void giveSched(ipengine::Core*) = 0;
	virtual void start() = 0;
};