#ifndef _INJECTOR_H_
#define _INJECTOR_H_

#include <boost/property_tree/xml_parser.hpp>
#include <boost/dll/import.hpp>
#include <boost/dll/alias.hpp>
#include <boost/function/function_base.hpp>
#include "DependencyGraph.h"
#include <IModule_API.h>
#include <IPCore/Core/ICore.h>
#include <iostream>
#include <algorithm>
#include <list>
#include <queue>
#include "XMLParser.h"
/*
Stuff to do:
	flags?
	-optional dependencies
	-updatable dependencies
	ERROR HANDLING
	-function to return all modules that can be used as dependency of type X

Stuff DONE:
-Read in a dependency graph from xml via property tree
-Get a path to look for dynamic libs to load
-start loading all dlibs in libraryfolder
-start building IModule_API objects, inject dependencies
-changing of dependency
	-callback in module_api
*/

class Injector
{
private:
	std::string libFolderPath;
	DGStuff::DependencyGraph depgraph;
	boost::property_tree::ptree tree;
	boost::property_tree::ptree *parsePropTreeFromXML(std::string path)
	{
		
		boost::property_tree::read_xml(path, tree, boost::property_tree::xml_parser::no_comments);//Check out what tree has in it if parsing fails
		//Parse propertytree into depgraph


		return &tree;
	};
	std::map<std::string, boost::shared_ptr<IModule_API>> loadedModules;
	std::map<std::string, boost::shared_ptr<IExtensionPoint>> loadedExtensions;
	std::vector<boost::filesystem::path> dlibFilePaths;
	bool parseDepGraphFromPropertyTree(boost::property_tree::ptree* tree);
	

	//Unnecessary, using std::find_if with lambda instead.
	DGStuff::Module* getModuleByIdentifier(std::vector<DGStuff::Module> *modules, std::string id);

	//What the fuck was this for again?
	boost::property_tree::ptree* getTreeByKeyValue(boost::property_tree::ptree* tree, std::string key, std::string value);


public:
	Injector()
	{
	
	};

	Injector(std::string dependencyXMLPath, std::string libraryFolderpath)
	{
		XMLParser parser{};
		auto g = parser.parse(dependencyXMLPath);
		if (parser.getResult() != DependencyParser::READING_SUCCESS)
		{
			//TODO
			//error handling
		}
		depgraph = *g; //? will this delete the object after g and thus the last shared_ptr is destroyed by exiting scope?
		
		 //if (parseDepGraphFromPropertyTree(parsePropTreeFromXML(dependencyXMLPath)))
		//{
		//	//TODO
		//	//do error handling and memes
		//}
		libFolderPath = libraryFolderpath;
	};

	void LoadModules(ipengine::Core* core, bool reload = false, std::string path = "");

	const std::map<std::string, boost::shared_ptr<IModule_API>>& getLoadedModules()
	{
		return loadedModules;
	}

	uint32_t reassignDependency(std::string moduleID, std::string dependencyID, std::string newModuleID)
	{
		//change dependency "dependencyID" in module "moduleID" to the module "newModuleID"
		//Do checks whether it's correct and then update the pointer in "moduleID"s moduleinfo of specified dependency
		//Tell "moduleID" to update it's pointers and do any work that's necessary for reassignment
		auto minfo = loadedModules[moduleID]->getModuleInfo();
		if (minfo->dependencies.exists(dependencyID) && minfo->dependencies.getDep<IModule_API>(dependencyID)->getModuleInfo()->iam == loadedModules[newModuleID]->getModuleInfo()->iam)
		{
			//reassignment should work, i think? 
			minfo->dependencies.assignDependency(dependencyID, loadedModules[newModuleID]);
			loadedModules[moduleID]->dependencyUpdated(dependencyID);
			return 1;
		}
		else
		{
			return 0;
		}
	}

	//Save the current dependency graph to it's source file it was loaded from
	bool saveDependencyGraph();
};

#endif