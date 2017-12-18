// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "DataStoreModule.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
DataStoreModule::DataStoreModule()
{
	m_info.identifier = "DataStoreModule";
	m_info.version = "1.0";
	m_info.iam = "IModule_API"; //Change this to your used API
	return;
}


bool DataStoreModule::startUp()
{
	//Initialize your module
	return true;
}

void DataStoreModule::set(const std::string key, const ipengine::any object)
{
	lock.lock();
	data[key] = ipengine::any(object);
	lock.unlock();
}

ipengine::any DataStoreModule::get(const std::string key)
{
	ipengine::any ret;
	lock.lock();
	if (data.find(key) != data.end())
	{
		ret = ipengine::any(data[key]);
	}

	lock.unlock();
	return ret;
}
