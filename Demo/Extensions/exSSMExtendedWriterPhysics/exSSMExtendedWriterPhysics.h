#ifndef _ExtensionTest_H_
#define _ExtensionTest_H_

#include <boost/config.hpp>
#include <IModule_API.h>
#include <IPhysicsModule_API.h>
#include <ISimpleSceneModule_API.h>
#include <boost/property_tree/xml_parser.hpp>
class exSSMExtendedWriterPhysics : public IExtension {
public:
	exSSMExtendedWriterPhysics();
	// TODO: add your methods here.
	virtual void execute(std::vector<std::string> argnames, std::vector<ipengine::any>& args) override;

	// Inherited via IExtension
	virtual ExtensionInformation * getInfo() override;

private:
	ExtensionInformation m_info;
	std::shared_ptr<IPhysicsModule_API> physics;
	bool init = false;
};

extern "C" BOOST_SYMBOL_EXPORT exSSMExtendedWriterPhysics extension;
exSSMExtendedWriterPhysics extension;

#endif