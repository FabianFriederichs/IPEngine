#include "IModule_API.h"

class IPrinter_API : public IModule_API
{
public:
	virtual void printStuffToSomething(std::string) = 0;
};