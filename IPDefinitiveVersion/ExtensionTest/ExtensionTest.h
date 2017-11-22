#ifndef _ExtensionTest_H_
#define _ExtensionTest_H_

#include <boost/config.hpp>
#include <IModule_API.h>
#include <ISimpleContentModule_API.h>
class ExtensionTest : public IExtensionPoint {
public:
	ExtensionTest();
	// TODO: add your methods here.
	virtual void execute(std::vector<std::string> argnames, std::vector<ipengine::any>& args) override;


	// Inherited via IExtensionPoint
	virtual ExtensionInformation * getInfo() override;
private:
	ExtensionInformation m_info;
};

extern "C" BOOST_SYMBOL_EXPORT ExtensionTest extension;
ExtensionTest extension;

#endif