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

bool Injector::recursiveInject(DGStuff::Module* mod)
{
	boost::shared_ptr<IExtensionPoint> pex;
	ExtensionInformation* pexinf;
	boost::shared_ptr<IModule_API> p;
	ModuleInformation* pinf;
	if (mod->isExtension)
	{
		pex = loadedExtensions[mod->identifier];
		pexinf = pex->getInfo();
	}
	else
	{
		p = loadedModules[mod->identifier];
		pinf = p->getModuleInfo();
	}
	for (auto dep : mod->dependencies)
	{
		auto d = dep->getModule();
		recursiveInject(d);

		if (dep->inject && !d->ignore) //!optional check, return false if not optional
		{
			auto injectee = loadedModules[d->identifier];
			if (injectee->isStartUp) //!same optional memes
			{
				if (mod->isExtension)
					pexinf->dependencies.assignDependency(dep->identifier, injectee);
				else
					pinf->dependencies.assignDependency(dep->identifier, injectee);
			}
		}
		//check inject status, check ignore/inject meme then inject, and startup stuff
	}

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
	if (mod->isExtension)
		return true;
	return p->isStartUp?true:p->startUp();
}


void Injector::LoadModules(ipengine::Core * core, std::string path, bool reload )
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
				auto tmp = boost::dll::import<IExtensionPoint>(path, "extension", boost::dll::load_mode::default_mode);
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

	//Iterate all loaded modules and inject memes.
	for (auto& dtmodule : depgraph->getRoots())
	{
		recursiveInject(dtmodule);
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
