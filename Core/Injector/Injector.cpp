#include "Injector.h"

bool Injector::recursiveInject(ipdg::Module* mod, bool doextension)
{
	
	std::shared_ptr<IExtension> pex;
	ExtensionInformation* pexinf;
	std::shared_ptr<IModule_API> p;
	ModuleInformation* pinf;
	ipengine::ErrorManager *errmanager;
	if(m_core)
		errmanager = &m_core->getErrorManager();
	if (!mod)
	{
		if (errmanager)
		{
			errmanager->reportException(ipengine::ipex("Injector::recursiveInject failed because supplied module node was null", ipengine::ipex_severity::warning));
		}
		return false;
	}
	if(mod->ignore)
		return true;
	if (mod->isExtension)
	{
		//!TODO Make sure the extension exists so no nullptr is used 
		if (loadedExtensions.count(mod->identifier) == 0 && !loadedExtensions[mod->identifier])
		{
			if(errmanager)
				errmanager->reportException(ipengine::ipex("Injector::recursiveInject failed because supplied module node did not have a corresponding module loaded", ipengine::ipex_severity::warning));
			return false;
		}
		pex = loadedExtensions[mod->identifier];
		pexinf = pex->getInfo();
		for (auto dep : mod->dependencies)
		{
			auto d = dep->getModule();
			bool ismandatory = false;
			if (pexinf->depinfo.count(dep->identifier) && pexinf->depinfo[dep->identifier].isMandatory)
			{
				ismandatory = true;
			}
			if (!d || (d && loadedModules.count(d->identifier) == 0 && !loadedModules[d->identifier]))
			{
				if (errmanager)
					errmanager->reportException(ipengine::ipex("Injector::recursiveInject encountered a problen. Dependency module node did not have a corresponding module loaded", ipengine::ipex_severity::warning));
				if(ismandatory)
					return false;
			}

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
				if (ismandatory)
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
		if (loadedModules.count(mod->identifier) == 0 && !loadedModules[mod->identifier])
		{
			if(errmanager)
				errmanager->reportException(ipengine::ipex("Injector::recursiveInject failed because supplied module node did not have a corresponding module loaded.", ipengine::ipex_severity::warning));
			return false;
		}
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

						if (e && !e->ignore && e->isExtension)
						{
							if (loadedExtensions.count(e->identifier) == 0 && !loadedExtensions[e->identifier])
							{
								if (errmanager)
									errmanager->reportException(ipengine::ipex("Injector::recursiveInject encountered an error. Extension module node did not have a corresponding extension loaded.", ipengine::ipex_severity::warning));
								return false;
							}
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
			bool ismandatory = false;
			if (pinf->depinfo.count(dep->identifier) && pinf->depinfo[dep->identifier].isMandatory)
			{
				ismandatory = true;
			}
			if (!d || (d && loadedModules.count(d->identifier) == 0 && !loadedModules[d->identifier]))
			{
				if (errmanager)
					errmanager->reportException(ipengine::ipex("Injector::recursiveInject encountered a problen. Dependency module node did not have a corresponding module loaded", ipengine::ipex_severity::warning));
				if(ismandatory)
					return false;
			}
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
				if (ismandatory)
				{
					return false;
				}
			}
			//check inject status, check ignore/inject meme then inject, and startup stuff
		}

		
		return safelyStartup(p, getModuleStartupErrorMessage(p->getModuleInfo()->identifier, "Injector::recursiveinject"));
	}
	
}

void Injector::LoadModule(ipengine::Core *core, std::string path)
{
	try {
		std::shared_ptr<boost::dll::shared_library> lib = std::make_shared<boost::dll::shared_library>(path, boost::dll::load_mode::default_mode);
		//boost::dll::shared_library lib(path, boost::dll::load_mode::default_mode);
		if (lib->has("module"))
		{
			//auto tmp = boost::dll::import<IModule_API>(path, "module", boost::dll::load_mode::default_mode);
			
			std::shared_ptr<IModule_API> tmp(lib, boost::addressof(lib->get<IModule_API>("module")));
			tmp->m_core = core;
			////!fallback name if identifier is empty

			loadedModules.insert({ tmp->getModuleInfo()->identifier, tmp });
		}
		//load extensions
		if (lib->has("extension"))
		{
			//auto tmp = boost::dll::import<IExtension>(path, "extension", boost::dll::load_mode::default_mode);
			//auto tmp = std::shared_ptr<IExtension>(&lib->get<IExtension>("module"));
			std::shared_ptr<IExtension> tmp(lib, boost::addressof(lib->get<IExtension>("extension")));

			tmp->m_core = core;
			////!fallback name if identifier is empty
			loadedExtensions.insert({ tmp->getInfo()->identifier, tmp });
		}
	}
	catch (std::exception ex)
	{
		std::string errmess("failed loading module at: " + path + "\nError message: " + ex.what());
		//!exception do stuff
		if (m_core)
		{
			m_core->getErrorManager().reportException(ipengine::ipex(errmess.c_str(), ipengine::ipex_severity::warning));
		}
		else
		{
			std::cout << errmess;
		}
	}
}

inline bool Injector::safelyStartup(std::shared_ptr<IModule_API> mod, std::string errorinfo)
{
	try {
		return mod->startUp();
	}
	catch (std::exception ex)
	{
		auto errmess = errorinfo.append(ex.what());
		if (m_core)
		{
			m_core->getErrorManager().reportException(ipengine::ipex(errmess.c_str(), ipengine::ipex_severity::error));
		}
		else
		{
			std::cout << errmess;
		}
		return false;
	}
}

inline bool Injector::safelyShutdown(std::shared_ptr<IModule_API> mod, std::string errorinfo)
{
	try {
		return mod->shutDown();
	}
	catch (std::exception ex)
	{
		auto errmess = errorinfo.append(ex.what());
		if (m_core)
		{
			m_core->getErrorManager().reportException(ipengine::ipex(errmess.c_str(), ipengine::ipex_severity::error));
		}
		else
		{
			std::cout << errmess;
		}
		return false;
	}
}

void Injector::registerCommands(ipengine::Core * core)
{
	auto& console = core->getConsole();
	console.addCommand("injector.loadmodule", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_loadModule>(this), "Loads a module from a shared library file. Arguments: filepath");
	console.addCommand("injector.reassign", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_reassignDep>(this), "(Re)Assigns a modules dependency. Arguments: targetmoduleid, targetdepid, newmoduleid");
	console.addCommand("injector.getloadedmodules", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_getLoadedModules>(this), "Prints a list of all loaded modules");
	console.addCommand("injector.getdeps", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_getDependencies>(this), "Prints a modules dependencies. Arguments: moduleid");
	console.addCommand("injector.getdepinfo", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_getDependencyInfo>(this), "Prints a modules dependency metadata. Arguments: moduleid");
	console.addCommand("injector.getmodulesoftype", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_getModulesOfType>(this), "Prints all modules that implement the supplied type. Arguments: moduletype");
	console.addCommand("injector.activateExt", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_enableExtension>(this), "(De)Activates a a modules extensions. Arguments: moduleid extensionpointid, prio, active(True|False)");
	console.addCommand("injector.removeDep", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_removeDependency>(this), "Removes a modules dependency. Arguments: moduleid dependencyid");
	console.addCommand("injector.sdmod", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_shutdownModule>(this), "Shutdown Module. Arguments: moduleid");
	console.addCommand("injector.sumod", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_startupModule>(this), "Startup Module. Arguments: moduleid");
	console.addCommand("inj.d", ipengine::CommandFunc::make_func<Injector, &Injector::cmd_debugswitchgraphics>(this), "Debug command");
}

inline Injector::Injector(ipengine::Core * core)
{
	m_core = core;
	if(core)
		registerCommands(core);
}

inline Injector::Injector(ipengine::Core * core, std::string dependencyXMLPath, std::string libraryFolderpath)
{
	m_core = core;
	if(core)
		registerCommands(core);
	XMLParser parser;
	bool failedparse = false;
	std::shared_ptr<ipdg::DependencyGraph> g;
	try {
		g = parser.parse(dependencyXMLPath);
	}
	catch (std::exception& ex)
	{
		failedparse = true;
		if (core)
		{
			core->getErrorManager().reportException(ipengine::ipex(ex.what(), ipengine::ipex_severity::warning, 0, "Injector::Injector(ipengine::Core, std::string, std::string"));
		}
		else
		{
			std::cout << ex.what();
		}
	}

	if (!failedparse) {
		depgraphpath = dependencyXMLPath;
		graphs[g] = depgraphpath;
		graphHasChanges[g] = false;
		depgraph = g;

		libFolderPath = libraryFolderpath;
	}
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


	////Check whether all modules in dependency graph are loaded
	//for(auto& mod : depgraph->getModules())
	//{
	//	if (mod.isExtension && loadedExtensions.find(mod.identifier)==loadedExtensions.end())
	//	{
	//		//! todo handle extension missing from direector
	//		//Check for optionality then either throw an error or do something else? Maybe set all modules with non optioonal dependency to to ignore so that they are not further loaded/started up?
	//	}
	//	else if (loadedModules.count(mod.identifier) == 0)
	//	{
	//		//! todo handle module 
	//	}
	//}

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

inline uint32_t Injector::reassignDependency(std::shared_ptr<IExtension> mod, std::string dependencyID, std::string newModuleID)
{
	//change dependency "dependencyID" in module "moduleID" to the module "newModuleID"
	//Do checks whether it's correct and then update the pointer in "moduleID"s moduleinfo of specified dependency
	//Tell "moduleID" to update it's pointers and do any work that's necessary for reassignment
	if (!mod.get())
		return 0;
	auto minfo = mod->getInfo();
	auto mod2 = loadedModules[newModuleID];
	auto minfo2 = mod2->getModuleInfo();
	if (loadedModules.count(newModuleID) < 1)
		return 0;
	auto location = "Injector::reassignDependency(IExtension)";

	std::shared_ptr<IModule_API> olddep = nullptr;
	//Case Dependee doesn't have the dependency yet, basically an assign. Check if it's in depinfo and if so check for compatability
	if (!minfo->dependencies.exists(dependencyID))
	{
		if (minfo->depinfo.count(dependencyID) > 0)
		{
			//Check whether dependency fits the depinfo and is valid
			if (minfo2->iam.find(minfo->depinfo[dependencyID].moduleType) != std::string::npos)
			{
				if (!mod2->isStartUp)
				{
					if (!safelyStartup(mod2, getModuleStartupErrorMessage(mod2->getModuleInfo()->identifier, location)))
						return 0;
				}
				minfo->dependencies.assignDependency(dependencyID, mod2);
				safelyDependencyUpdated(mod, dependencyID, olddep, getModuleDependencyChangedErrorMessage(minfo2->identifier, dependencyID));
				//Update dependencygraph accordingly
				depgraph->changeDependency(minfo->identifier, dependencyID, newModuleID);
				return 1;
			}
			else
			{
				//Module is not compatible, return with error
				return 0;
			}
		}
		else
		{
			if (!mod2->isStartUp)
			{
				if (!safelyStartup(mod2, getModuleStartupErrorMessage(mod2->getModuleInfo()->identifier, location)))
					return 0;
			}
			minfo->dependencies.assignDependency(dependencyID, mod2);
			safelyDependencyUpdated(mod, dependencyID, olddep, getModuleDependencyChangedErrorMessage(minfo2->identifier, dependencyID));
			//Update dependencygraph accordingly
			depgraph->changeDependency(minfo->identifier, dependencyID, newModuleID);
			return 1;
		}
	}
	else if (minfo2->iam.find(minfo->dependencies.getDep<IModule_API>(dependencyID)->getModuleInfo()->iam) != std::string::npos)
	{
		olddep = minfo->dependencies.getDep<IModule_API>(dependencyID);
		//reassignment should work, i think? 
		//Check whether dependency is updatable at runtime
		if (minfo->depinfo.count(dependencyID) && !minfo->depinfo[dependencyID].isUpdatable)
		{
			return 0;
		}

		//!TODO check newmoduleid for startedup and depinfo valid
		if (!mod2->isStartUp)
		{
			if (!safelyStartup(mod2, getModuleStartupErrorMessage(mod2->getModuleInfo()->identifier, location)))
				return 0;
		}

		minfo->dependencies.assignDependency(dependencyID, mod2);
		safelyDependencyUpdated(mod, dependencyID, olddep, getModuleDependencyChangedErrorMessage(minfo2->identifier, dependencyID));
		//Update dependencygraph accordingly
		depgraph->changeDependency(minfo->identifier, dependencyID, newModuleID);

		return 1;
	}
	else
	{
		return 0;
	}
}

inline bool Injector::startupModule(std::shared_ptr<IModule_API> mod)
{
	if (mod.get() && !mod->isStartUp)
		safelyStartup(mod, getModuleStartupErrorMessage(mod->getModuleInfo()->identifier, "Injector::startupModule"));
	if (mod.get())
		return mod->isStartUp;
	return false;
}

inline bool Injector::startupModule(std::string moduleID)
{
	if (loadedModules.count(moduleID) > 0)
	{
		return startupModule(loadedModules[moduleID]);
	}
	return false;
}

inline bool Injector::shutdownModule(std::shared_ptr<IModule_API> mod)
{
	if (mod.get() && mod->isStartUp)
		safelyShutdown(mod, getModuleShutdownErrorMessage(mod->getModuleInfo()->identifier, "Injector::shutdownModule"));
	if (mod.get())
		return !mod->isStartUp;
	return false;
}

inline bool Injector::shutdownModule(std::string moduleID)
{
	if (loadedModules.count(moduleID) > 0)
	{
		return shutdownModule(loadedModules[moduleID]);
	}
	return false;
}

inline uint32_t Injector::reassignDependency(std::shared_ptr<IModule_API> mod, std::string dependencyID, std::string newModuleID)
{
	//change dependency "dependencyID" in module "moduleID" to the module "newModuleID"
	//Do checks whether it's correct and then update the pointer in "moduleID"s moduleinfo of specified dependency
	//Tell "moduleID" to update it's pointers and do any work that's necessary for reassignment
	if (!mod.get())
		return 0;
	auto minfo = mod->getModuleInfo();
	auto mod2 = loadedModules[newModuleID];
	auto minfo2 = mod2->getModuleInfo();
	if (loadedModules.count(newModuleID) < 1)
		return 0;
	auto location = "Injector::reassignDependency(IModule_API)";
	std::shared_ptr<IModule_API> olddep = nullptr;
	if (!minfo->dependencies.exists(dependencyID))
	{

		if (minfo->depinfo.count(dependencyID) > 0)
		{
			//Check whether dependency fits the depinfo and is valid
			if (minfo2->iam.find(minfo->depinfo[dependencyID].moduleType) != std::string::npos)
			{
				if (!mod2->isStartUp)
				{

					if (!safelyStartup(mod2, getModuleStartupErrorMessage(mod2->getModuleInfo()->identifier, location)))
					{
						return 0;
					}

				}
				minfo->dependencies.assignDependency(dependencyID, mod2);
				safelyDependencyUpdated(mod, dependencyID, olddep, getModuleDependencyChangedErrorMessage(minfo2->identifier, dependencyID));
				//Update dependencygraph accordingly
				depgraph->changeDependency(minfo->identifier, dependencyID, newModuleID);
				return 1;
			}
			else
			{
				//Module is not compatible, return with error
				return 0;
			}
		}
		else
		{
			if (!mod2->isStartUp)
			{
				if (!safelyStartup(mod2, getModuleStartupErrorMessage(mod2->getModuleInfo()->identifier, location)))
				{
					return 0;
				}
			}
			minfo->dependencies.assignDependency(dependencyID, mod2);
			safelyDependencyUpdated(mod, dependencyID, olddep, getModuleDependencyChangedErrorMessage(minfo2->identifier, dependencyID));
			//Update dependencygraph accordingly
			depgraph->changeDependency(minfo->identifier, dependencyID, newModuleID);
			return 1;
		}
	}
	else if (minfo2->iam.find(minfo->dependencies.getDep<IModule_API>(dependencyID)->getModuleInfo()->iam) != std::string::npos)
	{
		olddep = minfo->dependencies.getDep<IModule_API>(dependencyID);
		//auto oldmod = minfo->dependencies.getDep<IModule_API>(dependencyID);
		//reassignment should work, i think? 
		//Check whether dependency is updatable at runtime
		if (minfo->depinfo.count(dependencyID) && !minfo->depinfo[dependencyID].isUpdatable)
		{
			return 0;
		}


		//if (depgraph->findModule(oldmod->getModuleInfo()->identifier)->getDepCounter() == 1)
		//{
		//	//Shutdown
		//	oldmod->shutDown();
		//}

		//!TODO check newmoduleid for startedup and depinfo valid
		if (!mod2->isStartUp)
		{
			if (!safelyStartup(mod2, getModuleStartupErrorMessage(mod2->getModuleInfo()->identifier, location)))
			{
				return 0;
			}
		}


		minfo->dependencies.assignDependency(dependencyID, mod2);
		safelyDependencyUpdated(mod, dependencyID, olddep, getModuleDependencyChangedErrorMessage(minfo2->identifier, dependencyID));
		//Update dependencygraph accordingly
		depgraph->changeDependency(minfo->identifier, dependencyID, newModuleID);

		return 1;
	}
	else
	{
		return 0;
	}
}

std::map<std::string, std::shared_ptr<IModule_API>> Injector::getModulesOfType(std::string type)
{
	// TODO: insert return statement here
	std::map<std::string, std::shared_ptr<IModule_API>> temp;

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

inline void Injector::cmd_startupModule(const ipengine::ConsoleParams & params)
{
	if (params.getParamCount() != 1)
	{
		m_core->getConsole().println("Parameter incorrect. One parameter: A valid module id");
	}
	//check path valid
	if (startupModule(params.get(0)))
	{
		m_core->getConsole().println(std::string("Module successfuly started.").c_str());
	}
	else
	{
		m_core->getConsole().println(std::string("Supplied module id not valid").c_str());
	}
}

inline void Injector::cmd_shutdownModule(const ipengine::ConsoleParams & params)
{
	if (params.getParamCount() != 1)
	{
		m_core->getConsole().println("Parameter incorrect. One parameter: A valid module id");
	}
	//check path valid
	if (shutdownModule(params.get(0)))
	{
		m_core->getConsole().println(std::string("Module successfuly shut down.").c_str());
	}
	else
	{
		m_core->getConsole().println(std::string("Supplied module id not valid").c_str());
	}
}

inline void Injector::cmd_loadModule(const ipengine::ConsoleParams & params)
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

inline void Injector::cmd_reassignDep(const ipengine::ConsoleParams & params)
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

inline void Injector::cmd_getLoadedModules(const ipengine::ConsoleParams & params)
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

inline void Injector::cmd_getDependencies(const ipengine::ConsoleParams & params)
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
			std::string output("\t" + dep.first + ": " + dep.second->getModuleInfo()->identifier);
			console.println(output.c_str());
		}
	}
	else
	{
		m_core->getConsole().println("Parameter incorrect. Fist paremeter is not a valid module id");
	}
}

inline void Injector::cmd_getDependencyInfo(const ipengine::ConsoleParams & params)
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
			std::string output("\t" + dep.first + ": \n\t\tType: " + dep.second.moduleType + "\n\t\tMandatory: " + (dep.second.isMandatory ? "true" : "false") + "\n\t\tUpdateable: " + (dep.second.isUpdatable ? "true" : "false"));
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

inline void Injector::cmd_getModulesOfType(const ipengine::ConsoleParams & params)
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

inline void Injector::cmd_removeDependency(const ipengine::ConsoleParams & params)
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

inline void Injector::cmd_enableExtension(const ipengine::ConsoleParams & params)
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

inline void Injector::cmd_debugswitchgraphics(const ipengine::ConsoleParams & params)
{
	auto &console = m_core->getConsole();
	auto targ = "VulkanRenderer";
	reassignDependency(loadedModules[targ], "WindowManager", "SDLWindowManager");
	reassignDependency(loadedModules[targ], "SCM", "SimpleContentModule");
	reassignDependency(loadedModules["GameLogicModule"], "graphics", targ);
	console.println("have fun");
}

bool Injector::saveDependencyGraph()
{
	XMLParser parser;
	try {
		return parser.write(*depgraph, depgraphpath) == DependencyParser::ParseResult::WRITING_SUCCESS ? true : false;
	}
	catch (std::exception ex)
	{
		std::string errmess("failed writing the dependency graph to file: " + depgraphpath + "\nError message: " + ex.what());
		//!exception do stuff
		if (m_core)
		{
			m_core->getErrorManager().reportException(ipengine::ipex(errmess.c_str(), ipengine::ipex_severity::warning));
		}
		else
		{
			std::cout << errmess;
		}
	}
	return false;
}

bool Injector::saveDependencyGraph(std::string path)
{
	try{
		XMLParser parser;
		return parser.write(*depgraph, path) == DependencyParser::ParseResult::WRITING_SUCCESS ? true : false;
	}
	catch (std::exception ex)
	{
		std::string errmess("failed writing the dependency graph to file: " + path + "\nError message: " + ex.what());
		//!exception do stuff
		if (m_core)
		{
			m_core->getErrorManager().reportException(ipengine::ipex(errmess.c_str(), ipengine::ipex_severity::warning));
		}
		else
		{
			std::cout << errmess;
		}
	}
	return false;
}

bool Injector::shutdown()
{
	bool completeshutdown = true;
	for (auto mod : loadedModules)
	{
		completeshutdown = shutdownModule(mod.second);
	}
	return completeshutdown;
}


