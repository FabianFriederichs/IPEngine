#include <boost/property_tree/xml_parser.hpp>
#include "DependencyGraph.h"
#include "IModule.h"
#include <iostream>
/*
Stuff to do:
Get a path to look for dynamic libs to load
Read in a dependency graph from xml via property tree
start building IModule objects, inject dependencies
*/

class Injector
{
private:
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
			tempModule.identifier = node.second.get<std::string>("name");
			if (tempModule.identifier.find("\n") == std::string::npos)
				return false;
			//Check for circular dependencies
			//Check whether the dependency is of a module already handles first then whether not and stuff
			tModules.push_back(tempModule);
			std::cout << node.first << std::endl;
			/*if (node.second.empty())
				continue;
			for (auto n2 : node.second)
			{
				std::cout << "    " << n2.first << std::endl;
				if (n2.second.empty())
					continue;
				for (auto n3 : n2.second)
				{
					std::cout << "        " << n3.first << std::endl;
					if (n3.second.empty())
						continue;
					for (auto n4 : n3.second)
					{
						if (n4.second.empty())
							std::cout << "            " << n4.first << ": " << n4.second.get_value<std::string>() << std::endl;
						if (n4.second.empty())
							continue;
						for (auto n5 : n4.second)
						{
							std::cout << "                " << n5.first << ": " << n5.second.get_value<std::string>() << std::endl;
						}
					}
				}
			}*/
		}
		return true;
	};

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

	Injector(std::string dependencyXMLPath, std::string libpath)
	{
		parseDepGraphFromPropertyTree(parsePropTreeFromXML(dependencyXMLPath));
	};
};