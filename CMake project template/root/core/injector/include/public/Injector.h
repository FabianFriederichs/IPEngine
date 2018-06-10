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
	std::shared_ptr<DGStuff::DependencyGraph> depgraph;
	std::unordered_map<std::shared_ptr<DGStuff::DependencyGraph>, std::string> graphs;
	std::unordered_map<std::shared_ptr<DGStuff::DependencyGraph>, bool> graphHasChanges;
	std::string depgraphpath;
	std::map<std::string, boost::shared_ptr<IModule_API>> loadedModules;
	std::map<std::string, boost::shared_ptr<IExtension>> loadedExtensions;
	std::vector<boost::filesystem::path> dlibFilePaths;	

	//Unnecessary, using std::find_if with lambda instead.
	DGStuff::Module* getModuleByIdentifier(std::vector<DGStuff::Module> *modules, std::string id);

	bool recursiveInject(DGStuff::Module*, bool = false);

	void LoadModule(ipengine::Core * core, std::string path);
	ipengine::Core *m_core;

public:
	void registerCommands(ipengine::Core *core);

	Injector(ipengine::Core *core)
	{
		m_core = core;
		registerCommands(core);
	};

	Injector(ipengine::Core *core, std::string dependencyXMLPath, std::string libraryFolderpath)
	{
		m_core = core;
		registerCommands(core);
		XMLParser parser;
		auto g = parser.parse(dependencyXMLPath);
		if (parser.getResult() != DependencyParser::READING_SUCCESS)
		{
			//TODO
			//error handling
		}
		
		
		depgraphpath = dependencyXMLPath;
		graphs[g] = depgraphpath;
		graphHasChanges[g] = false;
		depgraph = g;
		 //if (parseDepGraphFromPropertyTree(parsePropTreeFromXML(dependencyXMLPath)))
		//{
		//	//TODO
		//	//do error handling and memes
		//}
		libFolderPath = libraryFolderpath;
	};

	void LoadModules(std::string path = "", bool reload = false);

	const std::map<std::string, boost::shared_ptr<IModule_API>>& getLoadedModules()
	{
		return loadedModules;
	}

	uint32_t reassignDependency(boost::shared_ptr<IModule_API> mod, std::string dependencyID, std::string newModuleID)
	{
		//change dependency "dependencyID" in module "moduleID" to the module "newModuleID"
		//Do checks whether it's correct and then update the pointer in "moduleID"s moduleinfo of specified dependency
		//Tell "moduleID" to update it's pointers and do any work that's necessary for reassignment
		auto minfo = mod->getModuleInfo();
		if (loadedModules.count(newModuleID) < 1)
			return 0;
		if (loadedModules[newModuleID]->getModuleInfo()->iam.find(minfo->dependencies.getDep<IModule_API>(dependencyID)->getModuleInfo()->iam) != std::string::npos)
		{
			//reassignment should work, i think? 
			//Check whether dependency is updatable at runtime
			if (minfo->depinfo.count(dependencyID)&& !minfo->depinfo[dependencyID].isUpdatable)
			{
				return 0;
			}
			minfo->dependencies.assignDependency(dependencyID, loadedModules[newModuleID]);
			mod->dependencyUpdated(dependencyID);
			//Update dependencygraph accordingly
			depgraph->changeDependency(minfo->identifier, dependencyID, newModuleID);
		
			return 1;
		}
		else
		{
			return 0;
		}
	}

	uint32_t reassignDependency(boost::shared_ptr<IExtension> mod, std::string dependencyID, std::string newModuleID)
	{
		//change dependency "dependencyID" in module "moduleID" to the module "newModuleID"
		//Do checks whether it's correct and then update the pointer in "moduleID"s moduleinfo of specified dependency
		//Tell "moduleID" to update it's pointers and do any work that's necessary for reassignment
		auto minfo = mod->getInfo();
		if (loadedModules.count(newModuleID) < 1)
			return 0;
		if (minfo->dependencies.exists(dependencyID) && loadedModules[newModuleID]->getModuleInfo()->iam.find(minfo->dependencies.getDep<IModule_API>(dependencyID)->getModuleInfo()->iam)!=std::string::npos)
		{
			//reassignment should work, i think? 
			//Check whether dependency is updatable at runtime
			if (minfo->depinfo.count(dependencyID) && !minfo->depinfo[dependencyID].isUpdatable)
			{
				return 0;
			}
			minfo->dependencies.assignDependency(dependencyID, loadedModules[newModuleID]);
			mod->dependencyUpdated(dependencyID);
			//Update dependencygraph accordingly
			depgraph->changeDependency(minfo->identifier, dependencyID, newModuleID);
			return 1;
		}
		else
		{
			return 0;
		}
	}

	std::map<std::string, boost::shared_ptr<IModule_API>> getModulesOfType(std::string type);

	void cmd_loadModule(const ipengine::ConsoleParams& params)
	{
		if (params.getParamCount() != 1)
		{
			m_core->getConsole().println("Parameter incorrect. One parameter: A valid filepath");
		}
		//check path valid
		auto path = boost::filesystem::path(params.get(0));
		if (boost::filesystem::exists(path) && boost::filesystem::is_regular_file(path) && path.has_extension() && path.extension() == boost::dll::shared_library::suffix())
		{
			LoadModule(m_core, path.generic_string());
		}
		else
		{
			m_core->getConsole().println(std::string("Supplied path is not a valid " + boost::dll::shared_library::suffix().generic_string()).c_str());
		}
	}

	void cmd_reassignDep(const ipengine::ConsoleParams& params)
	{
		if (params.getParamCount() != 3)
		{
			m_core->getConsole().println("Parameter incorrect. Three parameters: A valid module id, dependencyid, module id");
		}
		//Get dependee (parameter 1) from string
		if (loadedModules.count(params.get(0)) > 0)
		{
			if (reassignDependency(loadedModules[params.get(0)], params.get(1), params.get(2)) == 1)
			{
				m_core->getConsole().println("Dependency updated successfully");
			}
			else
			{
				m_core->getConsole().println("Something went wrong updating the dependency. Either a parameter is wrong or the dependency supplied in parameter 2 is not updatable");
			}
		}
		else if (loadedExtensions.count(params.get(0)) > 0)
		{
			if (reassignDependency(loadedExtensions[params.get(0)], params.get(1), params.get(2)) == 1)
			{
				m_core->getConsole().println("Dependency updated successfully");
			}
			else
			{
				m_core->getConsole().println("Something went wrong updating the dependency. Either a parameter is wrong or the dependency supplied in parameter 2 is not updatable");
			}
		}
		else
		{
			m_core->getConsole().println("Parameter incorrect. Fist paremeter is not a valid module id");
		}
	}

	void cmd_getLoadedModules(const ipengine::ConsoleParams& params)
	{
		auto& modules = getLoadedModules();
		auto &console = m_core->getConsole();

		console.println("Modules:");
		for (auto& mod : modules)
		{
			auto minfo = mod.second->getModuleInfo();
			std::string mstrinfo("\t" + mod.first + ": " + minfo->identifier + " | " + minfo->iam + " | " + minfo->version);
			console.println(mstrinfo.c_str());
		}
		console.println("");
		console.println("Extensions:");
		for (auto& mod : loadedExtensions)
		{
			auto minfo = mod.second->getInfo();
			std::string mstrinfo("\t" + mod.first + ": " + minfo->identifier + " | " + minfo->version);
			console.println(mstrinfo.c_str());
		}
	}

	void cmd_getDependencies(const ipengine::ConsoleParams& params)
	{
		auto &console = m_core->getConsole();

		if (params.getParamCount() != 1)
		{
			console.println("Parameter incorrect. One parameter: A valid module id");
		}

		if (loadedModules.count(params.get(0)) > 0)
		{
			auto mod = loadedModules[params.get(0)];
			auto &deps = mod->getModuleInfo()->dependencies;
			console.println("Dependencies:");
			for (auto& dep : deps.dependencies)
			{
				std::string output("\t" + dep.first + ": " + dep.second->getModuleInfo()->identifier);
				console.println(output.c_str());
			}
		}
		else if (loadedExtensions.count(params.get(0)) > 0)
		{
			auto mod = loadedExtensions[params.get(0)];
			auto &deps = mod->getInfo()->dependencies;
			console.println("Dependencies:");
			for (auto& dep : deps.dependencies)
			{
				std::string output("\t"+dep.first + ": " + dep.second->getModuleInfo()->identifier);
				console.println(output.c_str());
			}
		}
		else
		{
			m_core->getConsole().println("Parameter incorrect. Fist paremeter is not a valid module id");
		}
	}

	void cmd_getDependencyInfo(const ipengine::ConsoleParams& params)
	{
		auto &console = m_core->getConsole();

		if (params.getParamCount() != 1)
		{
			console.println("Parameter incorrect. One parameter: A valid module id");
		}

		if (loadedModules.count(params.get(0)) > 0)
		{
			auto mod = loadedModules[params.get(0)];
			auto &deps = mod->getModuleInfo()->depinfo;
			console.println("Module supplied dependency information:");
			for (auto& dep : deps)
			{
				std::string output("\t" + dep.first + ": \n\t\tType: " + dep.second.moduleType + "\n\t\tMandatory: "+(dep.second.isMandatory?"true":"false") +"\n\t\tUpdateable: "+(dep.second.isUpdatable?"true":"false"));
				console.println(output.c_str());
			}
		}
		else if (loadedExtensions.count(params.get(0)) > 0)
		{
			auto mod = loadedExtensions[params.get(0)];
			auto &deps = mod->getInfo()->depinfo;
			console.println("Module supplied dependency information:");
			for (auto& dep : deps)
			{
				std::string output("\t" + dep.first + ": \n\t\tType: " + dep.second.moduleType + "\n\t\tMandatory: " + (dep.second.isMandatory ? "true" : "false") + "\n\t\tUpdateable: " + (dep.second.isUpdatable ? "true" : "false"));
				console.println(output.c_str());
			}
		}
		else
		{
			m_core->getConsole().println("Parameter incorrect. Fist paremeter is not a valid module id");
		}
	}

	void cmd_getModulesOfType(const ipengine::ConsoleParams& params)
	{
		auto &console = m_core->getConsole();
		if (params.getParamCount() != 1)
		{
			console.println("Parameter incorrect. One parameter: A module type");
		}

		auto modules = getModulesOfType(params.get(0));
		for (auto &mod : modules)
		{
			console.println(mod.first.c_str());
		}
		
	}

	void cmd_removeDependency(const ipengine::ConsoleParams& params)
	{
		auto &console = m_core->getConsole();
		if (params.getParamCount() != 2)
		{
			console.println("Parameter incorrect. three parameters: A Module ID, a Dependency ID and a bool");
		}
		auto modid = params.get(0);
		auto depid = params.get(1);

		if (loadedModules.count(modid) > 0)
		{
			auto modinfo = loadedModules[modid]->getModuleInfo();
			modinfo->dependencies.removeDependency(depid);
			
		}
		else if (loadedExtensions.count(modid) > 0)
		{
			auto modinfo = loadedExtensions[modid]->getInfo();
			modinfo->dependencies.removeDependency(depid);
		}
		else
		{
			console.println("Module not found");
		}

	}

	void cmd_enableExtension(const ipengine::ConsoleParams& params)
	{
		auto &console = m_core->getConsole();
		if (params.getParamCount() != 4)
		{
			console.println("Parameter incorrect. three parameters: A Module ID, a Dependency ID and a bool");
		}
		auto modid = params.get(0);
		auto exid = params.get(1);
		auto prio = params.getInt(2);
		auto active = params.get(3);
		if (loadedModules.count(modid) > 0)
		{
			auto modinfo = loadedModules[modid]->getModuleInfo();
			modinfo->expoints.setActive(exid, prio, active);
		}
		else
		{
			console.println("Module not found");
		}
	}
	
	

	//Save the current dependency graph to it's source file it was loaded from
	bool saveDependencyGraph();
	bool saveDependencyGraph(std::string);

	bool shutdown();
	bool shutdownModule(std::string);
};

#endif