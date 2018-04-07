#include "Injector.h"
// Injector.cpp : Defines the entry point for the console application.
//

DGStuff::Module * Injector::getModuleByIdentifier(std::vector<DGStuff::Module>* modules, std::string id)
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

bool Injector::recursiveInject(DGStuff::Module* mod, bool doextension)
{
	boost::shared_ptr<IExtension> pex;
	ExtensionInformation* pexinf;
	boost::shared_ptr<IModule_API> p;
	ModuleInformation* pinf;
	if (mod->isExtension)
	{
		//!TODO Make sure the extension exists so no nullptr is used 
		pex = loadedExtensions[mod->identifier];
		pexinf = pex->getInfo();
		for (auto dep : mod->dependencies)
		{
			auto d = dep->getModule();
			recursiveInject(d);
			auto injectee = loadedModules[d->identifier];
			auto depType = injectee->getModuleInfo()->iam;
			std::string wantedType = "";
			if (pexinf->depinfo.count(dep->identifier))
			{
				wantedType = pexinf->depinfo[dep->identifier].moduleType;
			}
			bool correctype = true;
			correctype = wantedType != "" ? (depType.find(wantedType) != std::string::npos) : true;
			if (dep->inject && injectee->isStartUp && !d->ignore && correctype)
			{
				pexinf->dependencies.assignDependency(dep->identifier, injectee);
			}
			else
			{
				//If dependency is mandatory and couldn't be assigned return without starting up the module
				//This doesn't really have an effect 
				if (pexinf->depinfo.count(dep->identifier) && pexinf->depinfo[dep->identifier].isMandatory)
				{
					return false;
				}
			}
		}
		pex->isActive = true;
		return true;
	}
	else
	{
		//!TODO Make sure the extension exists so no nullptr is used 

		p = loadedModules[mod->identifier];
		pinf = p->getModuleInfo();
		
		if (p->isStartUp)
		{
			if (doextension)
			{
				for (auto exp : mod->extensionpoints)
				{
					for (auto ext : exp->extensions)
					{
						auto e = ext.getModule();
						recursiveInject(e);

						if (!e->ignore && e->isExtension)
						{
							auto injectee = loadedExtensions[e->identifier];
							//injectee->isActive = ext.isActive; //! if I put active into depgraph
							pinf->expoints.assignExtension(exp->identifier, ext.priority, injectee);
						}
					}
				}
			}
			return true;
		}
			
		for (auto dep : mod->dependencies)
		{
			auto d = dep->getModule();
			recursiveInject(d);
			auto injectee = loadedModules[d->identifier];
			auto depType = injectee->getModuleInfo()->iam;
			std::string wantedType = "";
			if (pinf->depinfo.count(dep->identifier))
			{
				wantedType = pinf->depinfo[dep->identifier].moduleType;
			}
			bool correctype = true;
			correctype = wantedType != "" ? (depType.find(wantedType) != std::string::npos) : true;
			if (dep->inject && injectee->isStartUp && !d->ignore && correctype)
			{
				pinf->dependencies.assignDependency(dep->identifier, injectee);
			}
			else
			{
				//If dependency is mandatory and couldn't be assigned return without starting up the module
				if (pinf->depinfo.count(dep->identifier) && pinf->depinfo[dep->identifier].isMandatory)
				{
					return false;
				}
			}
			//check inject status, check ignore/inject meme then inject, and startup stuff
		}

		return p->startUp();
	}
	
}

void Injector::LoadModule(ipengine::Core *core, std::string path)
{
	try {
		boost::dll::shared_library lib(path, boost::dll::load_mode::default_mode);
		if (lib.has("module"))
		{
			auto tmp = boost::dll::import<IModule_API>(path, "module", boost::dll::load_mode::default_mode);
			tmp->m_core = core;
			//!fallback name if identifier is empty

			loadedModules.insert({ tmp->getModuleInfo()->identifier, tmp });
		}
		//load extensions
		if (lib.has("extension"))
		{
			auto tmp = boost::dll::import<IExtension>(path, "extension", boost::dll::load_mode::default_mode);
			tmp->m_core = core;
			//!fallback name if identifier is empty
			loadedExtensions.insert({ tmp->getInfo()->identifier, tmp });
		}
	}
	catch (std::exception ex)
	{
		//!exception do stuff
	}
}

void Injector::registerCommands(ipengine::Core * core)
{
	auto& console = core->getConsole();
	console.addCommand("injector.loadmodule", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_loadModule>(this), "ech filepath");
	console.addCommand("injector.reassign", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_reassignDep>(this), "ech targetmoduleid targetdepid newdependencyid");
	console.addCommand("injector.getloadedmodules", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_getLoadedModules>(this), "ech");
	console.addCommand("injector.getdeps", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_getDependencies>(this), "ech moduleid");
	console.addCommand("injector.getdepinfo", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_getDependencyInfo>(this), "ech moduleid");
	console.addCommand("injector.getmodulesoftype", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_getModulesOfType>(this), "ech moduletype");
	console.addCommand("injector.activateExt", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_enableExtension>(this), "ech modid exid prio active");
	console.addCommand("injector.removeDep", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_removeDependency>(this), "ech modid depid");
}

void Injector::LoadModules(std::string path, bool reload )
{
	//Check if given path is valid. If it is not previous path or local will be used for dll location
	bool newPath = false;
	if (path != "" && boost::filesystem::exists(path))
	{
		newPath = true;
		libFolderPath = path;
	}

	if (reload)
	{
		//TODO
		//!delete modules and shit
	}

	auto workingpath = boost::filesystem::current_path();
	boost::filesystem::directory_iterator di(boost::filesystem::system_complete(newPath ? path : (libFolderPath == "" ? boost::filesystem::current_path() : libFolderPath)));

	//Iterate directory and find all files with the systems shared library extension (dll/so)
	for (auto f : di)
	{
		if (boost::filesystem::is_regular_file(f.path()) && f.path().has_extension() && f.path().extension() == boost::dll::shared_library::suffix())
		{
			dlibFilePaths.push_back(f.path());
		}
	}

	//load modules from the found library paths. 
	for (auto path : dlibFilePaths)
	{
		LoadModule(m_core, path.generic_string());
	}


	//Check whether all modules in dependency graph are loaded
	for(auto& mod : depgraph->getModules())
	{
		if (mod.isExtension && loadedExtensions.find(mod.identifier)==loadedExtensions.end())
		{
			//! todo handle extension missing from direector
			//Check for optionality then either throw an error or do something else? Maybe set all modules with non optioonal dependency to to ignore so that they are not further loaded/started up?
		}
		else if (loadedModules.find(mod.identifier) == loadedModules.end())
		{
			//! todo handle module 
		}
	}

	//!!!Check for circular dependencies
	bool incompleteInject = false;
	//auto tcopy = depgraph->deepCopy();
	//Iterate all loaded modules and inject memes.
	for (auto& dtmodule : depgraph->getRoots())
	{
		//Optional dependencies are not guaranteed to be started up and injected
		if (incompleteInject = !recursiveInject(dtmodule))
		{
			//Handle case of root module not being able to startup because of missing dependancy
			//!
		}
	}
	for (auto& dtmodule : depgraph->getModules())
	{
		//Optional dependencies are not guaranteed to be started up and injected
		if (incompleteInject = !recursiveInject(&dtmodule, true))
		{
			//Handle case of root module not being able to startup because of missing dependancy
			//!
		}
	}
	if (incompleteInject)
	{
		//graphs[std::make_shared<DGStuff::DependencyGraph>(tcopy)] = depgraphpath;
		graphHasChanges[depgraph] = incompleteInject;
		
	}
}

std::map<std::string, boost::shared_ptr<IModule_API>> Injector::getModulesOfType(std::string type)
{
	// TODO: insert return statement here
	std::map<std::string, boost::shared_ptr<IModule_API>> temp;

	for (auto& m : loadedModules)
	{
		//Find modules that ahve listed type in their "iam"
		if (m.second->getModuleInfo()->iam.find(type) != std::string::npos)
		{
			temp.insert({ m.first, m.second });
		}
	}
	return temp;
}

bool Injector::saveDependencyGraph()
{
	XMLParser parser;
	return parser.write(*depgraph, depgraphpath) == DependencyParser::ParseResult::WRITING_SUCCESS ? true : false;

}

bool Injector::saveDependencyGraph(std::string path)
{
	XMLParser parser;
	return parser.write(*depgraph, path) == DependencyParser::ParseResult::WRITING_SUCCESS ? true : false;
}

bool Injector::shutdown()
{
	bool completeshutdown = true;
	for (auto mod : loadedModules)
	{
		completeshutdown = mod.second->shutDown();
	}
	return completeshutdown;
}

bool Injector::shutdownModule(std::string identifier)
{
	return loadedModules.find(identifier)!=loadedModules.end()?loadedModules[identifier]->shutDown() : false;
}
