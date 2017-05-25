#include "IModule_API.h"

class IPrinter : public IModule
{
public:
	virtual void printStuffToSomething(std::string) = 0;
};