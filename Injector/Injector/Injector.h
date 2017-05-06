#include <boost/property_tree/xml_parser.hpp>
#include "DependencyGraph.h"
#include "IModule.h"
#include <iostream>
#include <algorithm>
#include <list>
/*
Stuff to do:
start loading all dlibs in libraryfolder
start building IModule objects, inject dependencies

Stuff DONE:
-Read in a dependency graph from xml via property tree
-Get a path to look for dynamic libs to load
*/

class Injector
{
private:
	std::string libFolderPath;
	DGStuff::DependencyGraph depgraph;
	boost::property_tree::ptree tree;
	boost::property_tree::ptree *parsePropTreeFromXML(std::string path)
	{
		
		boost::property_tree::read_xml(path, tree);//Check out what tree has in it if parsing fails
		//Parse propertytree into depgraph


		return &tree;
	};

	bool parseDepGraphFromPropertyTree(boost::property_tree::ptree* tree)
	{
	//first iteration create all modules	
		std::vector<DGStuff::Module> tModules;
		for (auto node : tree->get_child("DependencyGraph.Modules"))
		{
			DGStuff::Module tempModule;
			tempModule.identifier = node.second.get<std::string>("identifier");
			if (tempModule.identifier.find("\n") != std::string::npos)
				return false;
			//Check for circular dependencies
			//Check whether the dependency is of a module already handles first then whether not and stuff
			tModules.push_back(tempModule);
		}

		std::list<DGStuff::Module*> dgRoots;
		std::vector<std::string> nonRootIds;
		for (auto node : tree->get_child("DependencyGraph.Modules"))
		{
			
			if (node.second.to_iterator(node.second.find("dependencies"))!=node.second.end() && node.second.get_child("dependencies").count("dependency") > 0)
			{
				std::string id = node.second.get<std::string>("identifier");
				DGStuff::Module *module = &*std::find_if(tModules.begin(), tModules.end(), [id](DGStuff::Module v)->bool{return v.identifier == id; }); //getModuleByIdentifier(&tModules, id);
				dgRoots.push_back(module);               
				for (auto dependencyNode : node.second.get_child("dependencies"))
				{
					std::string idD = dependencyNode.second.get<std::string>("depID");
					auto memes = std::find_if(tModules.begin(), tModules.end(), [idD](DGStuff::Module v)->bool{return v.identifier == idD; });//getModuleByIdentifier(&tModules, idD);
					if (memes == tModules.end())
					{
						return false; //If module is not found
					}
					nonRootIds.push_back(idD);
					module->dependencies[dependencyNode.second.get<std::string>("identifier")] = &*memes;
					 
					//dgRoots.remove_if([memes](DGStuff::Module* v)->bool{return v == memes; })
					//Optional. Only removes those modules from dgRoots that have already been handled. 
					if (std::find(dgRoots.begin(), dgRoots.end(), &*memes) != dgRoots.end())
					{
						dgRoots.remove(&*memes);
					}
				}
			}
		}
		for (auto m : dgRoots)
		{
			if (std::find(nonRootIds.begin(), nonRootIds.end(), m->identifier)== nonRootIds.end())
			{
				depgraph.addModule(*m);
			}
		}
		return true;
	};

	//Unnecessary, using std::find_if with lambda instead.
	DGStuff::Module* getModuleByIdentifier(std::vector<DGStuff::Module> *modules, std::string id)
	{
		for (auto& m : *modules)
		{
			if (m.identifier == id)
			{
				return &m; //returning address of something that won't exist afterwards anymore WRONG
			}
		}
		return nullptr;
	}

	//What the fuck was this for again?
	boost::property_tree::ptree* getTreeByKeyValue(boost::property_tree::ptree* tree, std::string key, std::string value)
	{
		for (auto node : *tree)
		{
			if (node.second.to_iterator(node.second.find(key)) == node.second.end())
			{
				return nullptr; //Key doesn't even exist
			}
			if (node.second.get<std::string>(key) == value)
			{
				return &node.second;
			}
		}
		return nullptr;
	}

public:
	Injector()
	{
	
	};

	Injector(std::string dependencyXMLPath, std::string libraryFolderpath)
	{
		if (parseDepGraphFromPropertyTree(parsePropTreeFromXML(dependencyXMLPath)))
		{
			//do error handling and memes
		}
		libFolderPath = libraryFolderpath;
	};
};