// Plugin2.cpp : Defines the exported functions for the DLL application.
//1
#include "exSSMExtendedPBRPhysicsLoader.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
exSSMExtendedPBRPhysicsLoader::exSSMExtendedPBRPhysicsLoader()
{
	m_info.identifier = "exSSMExtendedPBRPhysicsLoader";

	//m_info.
}


void exSSMExtendedPBRPhysicsLoader::execute(std::vector<std::string> argnames, std::vector<ipengine::any>& args)
{

	if (argnames.size() < 1 || argnames[0] != "this")
	{
		return;
	}
	if (argnames.size() <2 || argnames[1] != "type")
	{
		return;
	}
	if (argnames.size() <3 || argnames[1] != "tree")
	{
		return;
	}if (argnames.size() <4 || argnames[1] != "entity")
	{
		return;
	}if (argnames.size() <5 || argnames[1] != "contentmodule")
	{
		return;
	}if (argnames.size() <6 || argnames[1] != "entitymap")
	{
		return;
	}

	auto ssm = args[0].cast<ISimpleSceneModule_API*>();
	auto type = args[1].cast<std::string>();
	auto tree = args[2].cast<boost::property_tree::ptree>();
	auto entity = args[3].cast<SCM::Entity*>();
	auto scm = args[4].cast<SCM::ISimpleContentModule_API>();
	auto entitymap = args[5].cast<std::unordered_map<std::string, SCM::Entity*>>();

	if (type == "physicsCloth")
	{

	}
	else if (type == "dirlight")
	{

	}
	else if (type == "pointlight")
	{

	}
	else if (type == "spotlight")
	{

	}
}

ExtensionInformation * exSSMExtendedPBRPhysicsLoader::getInfo()
{
	return &m_info;
}
