#ifndef _ExtensionTest_H_
#define _ExtensionTest_H_

#include <boost/config.hpp>
#include <IModule_API.h>
#include <ISimpleContentModule_API.h>
#include <IGraphics_API.h>
class ExtensionTest : public IExtension {
public:
	ExtensionTest();
	// TODO: add your methods here.
	virtual void execute(std::vector<std::string> argnames, std::vector<ipengine::any>& args) override;


	// Inherited via IExtension
	virtual ExtensionInformation * getInfo() override;
	void changeCamera(ipengine::TaskContext& c);
private:
	ExtensionInformation m_info;
	std::vector<ipengine::Scheduler::SubHandle> handle;
	ipengine::ipid cameraid;
	bool init = false;
};

extern "C" BOOST_SYMBOL_EXPORT ExtensionTest extension;
ExtensionTest extension;

#endif