#ifndef _IDataStoreModuleh_API_H_
#define _IDataStoreModuleh_API_H_

#include <IModule_API.h>


class IDataStoreModuleh_API : public IModule_API {
public:
	virtual void set(const std::string key, const ipengine::any object) = 0;
	virtual ipengine::any get(const std::string key) = 0;
private:
	
};

#endif
