#ifndef _ExtensionTest_H_
#define _ExtensionTest_H_

#include <boost/config.hpp>
#include <IModule_API.h>
#include <ISimpleSceneModule_API.h>
#include <IPhysicsModule_API.h>
#include <ISimpleContentModule_API.h>
#include <boost/property_tree/xml_parser.hpp>
class exSSMExtendedPBRPhysicsLoader : public IExtension {
public:
	exSSMExtendedPBRPhysicsLoader();
	// TODO: add your methods here.
	virtual void execute(std::vector<std::string> argnames, std::vector<ipengine::any>& args) override;

	// Inherited via IExtension
	virtual ExtensionInformation * getInfo() override;

private:
	ExtensionInformation m_info;
	//boost::shared_ptr<ISimpleSceneModule_API> ssm;
	boost::shared_ptr<ISimpleSceneModule_API> physics;
	boost::shared_ptr<ISimpleSceneModule_API> scm;
};

extern "C" BOOST_SYMBOL_EXPORT exSSMExtendedPBRPhysicsLoader extension;
exSSMExtendedPBRPhysicsLoader extension;

#endif