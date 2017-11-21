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
	bool parseDepGraphFromPropertyTree(boost::property_tree::ptree* tree)
	{
	//first iteration create all modules	
		//std::list<DGStuff::Module*> tModules;
		for (auto node : tree->get_child("DependencyGraph.Extensions"))
		{
			DGStuff::Extension tempExt;
			tempExt.inject = node.second.get("inject", true);
			tempExt.identifier = node.second.get<std::string>("identifier");
			if (tempExt.identifier.find("\n") != std::string::npos)
				return false;
			//TODO
			//Check for circular dependencies
			//Check whether the dependency is of a module already handles first then whether not and stuff

			depgraph.addExtension(tempExt);
			//tModules.push_back(&depgraph.getModules()->back());
		}
		auto depexts = depgraph.getExtensions();
		for (auto node : tree->get_child("DependencyGraph.Modules"))
		{
			DGStuff::Module tempModule;
			if (node.second.get("ignore", false))
				continue;
			tempModule.identifier = node.second.get<std::string>("identifier");
			if (tempModule.identifier.find("\n") != std::string::npos)
				return false;
			if (node.second.get_child_optional("ExtensionPoints"))
			{
				for (auto node2 : node.second.get_child("ExtensionPoints"))
				{
					DGStuff::ExtensionPoint p;
					p.extensionpointidentifier = node2.second.get<std::string>("identifier", "");
					for (auto mextensions : node2.second.get_child("Extensions"))
					{
						std::string text;
						text = mextensions.second.get<std::string>("extensionname", "");
						auto tempext = std::find_if(depexts->begin(), depexts->end(), [text](DGStuff::Extension e)->bool {return e.identifier == text; });
						if (text == "" || tempext == depexts->end())
							continue;
						uint32_t prio = mextensions.second.get<uint32_t>("priority", 0);
						while (p.extensions.count(prio) > 0)
						{
							++prio;
						}
						p.extensions[prio] = &*tempext;
					}
					tempModule.extensions.push_back(p);
			}
		}
			//TODO
			//Check for circular dependencies
			//Check whether the dependency is of a module already handles first then whether not and stuff
			
			depgraph.addModule(tempModule);
			//tModules.push_back(&depgraph.getModules()->back());
		}

		auto tModules = depgraph.getModules();
		std::list<DGStuff::Module*> dgRoots;
		std::vector<std::string> nonRootIds;
		for (auto node : tree->get_child("DependencyGraph.Modules"))
		{
			if (node.second.get("ignore", false))
				continue;
			if (node.second.to_iterator(node.second.find("dependencies"))!=node.second.end() && node.second.get_child("dependencies").count("dependency") > 0)
			{
				std::string id = node.second.get<std::string>("identifier");
				DGStuff::Module *module = &*std::find_if(tModules->begin(), tModules->end(), [id](DGStuff::Module v)->bool{return v.identifier == id; }); //getModuleByIdentifier(&tModules, id);
				dgRoots.push_back(module);               
				for (auto dependencyNode : node.second.get_child("dependencies"))
				{
					bool inject = dependencyNode.second.get("inject", true);
					
					std::string idD = dependencyNode.second.get<std::string>("moduleID");
					auto memes = std::find_if(tModules->begin(), tModules->end(), [idD](DGStuff::Module v)->bool{return v.identifier == idD; });//getModuleByIdentifier(&tModules, idD);
					if (inject && memes == tModules->end())
					{
						return false; //If module is not found
					}
					nonRootIds.push_back(idD);
					std::string depIdent = dependencyNode.second.get<std::string>("identifier");
					module->dontInject[depIdent] = inject;
					if (memes == tModules->end())
						module->dependencies[depIdent] = nullptr;
					else
					{
						module->dependencies[depIdent] = &*memes;

						//dgRoots.remove_if([memes](DGStuff::Module* v)->bool{return v == memes; })
						//Optional. Only removes those modules from dgRoots that have already been handled. 
						if (std::find(dgRoots.begin(), dgRoots.end(), &*memes) != dgRoots.end())
						{
							dgRoots.remove(&*memes);
						}
					}
				}
			}
		}
		for (auto& m : *tModules)
		{
			//depgraph.addModule(m);
			auto meme = std::find(nonRootIds.begin(), nonRootIds.end(), m.identifier);
			if (meme == nonRootIds.end())
			{
				depgraph.addRoot(&m);
			}
			
		}
//		depgraph.getModules()->front().iname = "test"; //i'm too retarded for pointers 
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
			//TODO
			//do error handling and memes
		}
		libFolderPath = libraryFolderpath;
	};

	void LoadModules(ipengine::Core* core, bool reload = false, std::string path = "")
	{
		bool newPath = false;
		if (path != "" && boost::filesystem::exists(path))
		{
			newPath = true;
			libFolderPath = path;
		}

		if (reload)
		{
			//TODO
			//delete modules and shit
		}
		
		auto meme = boost::filesystem::current_path();
		boost::filesystem::directory_iterator di(boost::filesystem::system_complete(newPath?path:(libFolderPath==""?boost::filesystem::current_path():libFolderPath)));

		for (auto f : di)
		{
			if (boost::filesystem::is_regular_file(f.path()) && f.path().has_extension() && f.path().extension() == boost::dll::shared_library::suffix())
			{
				dlibFilePaths.push_back(f.path());
			}
		}
			//load modules from the found library paths. Append or Replace current list? Probably append
		for (auto path : dlibFilePaths)
		{
			try{
				boost::dll::shared_library lib(path, boost::dll::load_mode::default_mode);
				if (lib.has("module"))
				{				//bo
				//boost::shared_ptr<IModule_API> meme = lib.get<boost::shared_ptr<IModule_API>>("module");
					loadedModules[path.filename().stem().generic_string()] = boost::dll::import<IModule_API>(path, "module", boost::dll::load_mode::default_mode);// lib.get<boost::shared_ptr<IModule_API>>("module"); //boost::dll::import_alias<IModule_API>(boost::move(lib), "module");
					loadedModules[path.filename().stem().generic_string()]->m_core = core;
				}
				//load extensions
				if (lib.has("extension"))
				{
					loadedExtensions[path.filename().stem().generic_string()] = boost::dll::import<IExtensionPoint>(path, "extension", boost::dll::load_mode::default_mode);
				}
			}
			catch (std::exception ex)
			{
				//do stuff
			}
			//loadedModules[path.filename().stem().generic_string()] = boost::dll::import<IModule_API>(path,"module", boost::dll::load_mode::default_mode);
		}

		
		//Check whether all modules in dependency graph are loaded
		auto& tmp = loadedModules;
		if (!std::all_of(depgraph.getModules()->begin(), depgraph.getModules()->end(), [tmp](DGStuff::Module &v)->bool{return tmp.count(v.identifier) > 0; }))
		{
			//TODO
			//Handle case of missing modules found in dependency graph
		}

		////Iterate all loaded modules and inject memes.
		//for (auto mkv: loadedModules)
		//{
		//	/*
		//	Approach:
		//	Iterate through nodes of depth x where x=max depth and reduce x on each iteration
		//	*/
		//	auto module = mkv.second;



		//}
		//Iterate through the dep graph in reverse, nodes of one depth etc
		std::vector<std::string> processedModules;
		std::queue<const DGStuff::Module*> toProcess;
		std::list<const DGStuff::Module*> injectOrderList;
		//for (auto mnd = )
		for (auto&& modnode : *(depgraph.getRoots()))
		{
			/*depgraph.forEachReverse(&modnode, [&processedModules, &tmp](const DGStuff::Module *v)->
				void
				{
					if (std::find(processedModules.begin(), processedModules.end(), v->identifier) == processedModules.end())
					{

					}
				});*/
			//breadth-first search and add on a stack
			toProcess.push(modnode);
			injectOrderList.push_back(modnode);
		}

		while (!toProcess.empty())
		{
			auto m = toProcess.front();
			toProcess.pop();
			for (auto modnode : m->dependencies)
			{
				if (modnode.second != nullptr)
				{
					toProcess.push(modnode.second);
					injectOrderList.push_back(modnode.second);
				}
			}
		}
		injectOrderList.reverse();
		injectOrderList.unique();

		for (auto modnode : injectOrderList)
		{
			auto info = loadedModules[modnode->identifier]->getModuleInfo();

			if (!modnode->dependencies.empty())
			{
				assert(info->dependencies.size() == 0);
				for (auto mn : modnode->dependencies)
				{ 
					if (modnode->dontInject.at(mn.first))
						info->dependencies.assignDependency(mn.first, loadedModules[mn.second->identifier]);
				}
			}
			for (auto e : modnode->extensions)
			{
				for (auto e2 : e.extensions)
				{
					if (e2.second->inject)
					{
						if (loadedExtensions.count(e2.second->identifier) > 0)
						{
							info->expoints.expoints[e.extensionpointidentifier].push_back(loadedExtensions[e2.second->identifier]);
							loadedExtensions[e2.second->identifier]->isActive = true;
						}
					}
				}
			}
			loadedModules[modnode->identifier]->startUp();
		}
	}

	const std::map<std::string, boost::shared_ptr<IModule_API>>& getLoadedModules()
	{
		return loadedModules;
	}

	void reassignDependency(std::string moduleID, std::string dependencyID, std::string newModuleID)
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
		}
	}

};