#ifndef _DataStoreModule_H_
#define _DataStoreModule_H_

#include <boost/config.hpp>
#include <IDataStoreModuleh_API.h>

class DataStoreModule : public IDataStoreModuleh_API {
public:
	DataStoreModule();
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }

	// Inherited via IDataStoreModuleh_API
	virtual void set(const std::string key, const ipengine::any object) override;

	virtual ipengine::any get(const std::string key) override;
private:
	ModuleInformation m_info;

	std::map<std::string, ipengine::any> data;

	ipengine::BasicSpinLock lock;
	


	// Inherited via IDataStoreModuleh_API
	virtual bool _startup() override;


	// Inherited via IDataStoreModuleh_API
	virtual bool _shutdown() override;

};

extern "C" BOOST_SYMBOL_EXPORT DataStoreModule module;
DataStoreModule module;

#endif