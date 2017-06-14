#include "IModule_API.h"
#include <map>
#include <boost\any.hpp>
#include <vector>
class IInput_API : public IModule_API
{
public:
	using DataMap = std::map<std::string, boost::any>;
	
	//virtual void printStuffToSomething(std::string) = 0;
	virtual std::vector<DataMap> getInputBuffered() = 0;
};