/** \addtogroup Injector
Dependency Injection realizing components
*  @{
*/
/*!
\file Injector.h
\brief Defines the injector
*/

#ifndef _INJECTOR_H_
#define _INJECTOR_H_

#include <boost/property_tree/xml_parser.hpp>
#include <boost/dll/import.hpp>
#include <boost/dll/alias.hpp>
#include <boost/function/function_base.hpp>
#include "DependencyGraph.h"
#include <IModule_API.h>
//#include <IPCore/Core/ICore.h>
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


/*!
\brief Manages all dependency logic. From parsing and saving a dependency graph to loading and starting up all modules described in the graph.

The Injector class is used to load modules from shared libraries (dll/so), inject their dependencies according to a DependencyGraph provided via an xml file and start them up.
It also keeps track of all changes done to the graph from deletions/reassignments of dependencies.
Furthermore extensions are handled parallel to modules and "injected" as described in the graph.

A valid instance of an ipengine::Core object is necessary for the injector to work properly.

It is possible to change module dependencies at runtime, as long as a module doesn't describe a dependency as unchangeable and implements the dependencyChanged function to fix its state.
The Injector is not yet fully "smart" in that Shutdown and Startup of individual modules doesn't yet take into account its dependency metadata or DependencyGraph and they should be used very carefully as they can cause modules to run in faulty states.


Various console commands are registered by the injector on construction to interact with the dependency system.
These include:
\li injector.loadmodule: Loads a module from a shared library file. Arguments: filepath
\li injector.reassign: (Re)Assigns a modules dependency. Arguments: targetmoduleid, targetdepid, newmoduleid
\li injector.getloadedmodules: Prints a list of all loaded modules
\li injector.getdeps: Prints a modules dependencies. Arguments: moduleid
\li injector.getdepinfo: Prints a modules dependency metadata. Arguments: moduleid
\li injector.getmodulesoftype: Prints all modules that implement the supplied type. Arguments: moduletype
\li injector.activateExt: (De)Activates a a modules extensions. Arguments: moduleid extensionpointid, prio, active(True|False)
\li injector.removeDep: Removes a modules dependency. Arguments: moduleid dependencyid
\li injector.sdmod: Shutdown Module. Arguments: moduleid
\li injector.sumod: Startup Module. Arguments: moduleid
*/
class Injector
{
	
public:

	/*!
	\brief Basic Injector constructor. Does no injection work.

	\param[in] core		The Core instance that will be worked with for error handling and injection
	*/
	Injector(ipengine::Core *core);;

	/*!
	\brief Injector constructor that loads and starts up modules.

	\param[in] core		The Core instance that will be worked with for error handling and injection
	\param[in] dependencyXMLPath	The filepath of the DependencyGraph
	\param[in] libraryFolderpath	Path to the folder containing modules
	*/
	Injector(ipengine::Core *core, std::string dependencyXMLPath, std::string libraryFolderpath);;

	/*!
	\brief Loads all shared libraries containing a "module" symbol from a given path

	\param[in] path		The path to a folder containing shared libraries to load
	*/
	void LoadModules(std::string path = "", bool reload = false);
	/*!
	\brief Loads a module from a given shared library file path

	\param[in] core		The Core instance that will be worked with for error handling and injection
	\param[in] path		The filepath of a shared library containing the module symbol
	*/
	void LoadModule(ipengine::Core * core, std::string path);

	/*!
	\brief Returns a map with all loaded modules

	\param[out]		A map containing all loaded modules assigned to their identifier
	*/
	const std::map<std::string, std::shared_ptr<IModule_API>>& getLoadedModules()
	{
		return loadedModules;
	}

	/*!
	\brief (Re)Assigns a different module to a modules dependency
	This takes into account the dependency meta data on whether a module fits the dependency

	\param[in] mod		The module whose dependency should be changed
	\param[in] dependencyID		The identifier of the dependency that should be changed
	\param[in] newModuleID		The identifier of the module that should be assigned to the dependency
	*/
	uint32_t reassignDependency(std::shared_ptr<IModule_API> mod, std::string dependencyID, std::string newModuleID);

	/*!
	\brief (Re)Assigns a different module to a modules dependency
	This takes into account the dependency meta data on whether a module fits the dependency

	\param[in] mod		The IExtension whose dependency should be changed
	\param[in] dependencyID		The identifier of the dependency that should be changed
	\param[in] newModuleID		The identifier of the module that should be assigned to the dependency
	*/
	uint32_t reassignDependency(std::shared_ptr<IExtension> mod, std::string dependencyID, std::string newModuleID);

	/*!
	\brief Starts a module up by calling its startUp funtion.

	This can be called to start a module up.
	This does not however take into account whether the modules dependencies are valid and might fail or cause instability because the module ends up running in a faulty state

	\param[in] mod		The module to start up
	*/
	bool startupModule(std::shared_ptr<IModule_API> mod);
	/*!
	\brief Starts a module up by calling its startUp funtion.

	\param[in] mod		The identifier of the module to start up
	*/
	bool startupModule(std::string moduleID);

	/*!
	\brief Shuts down a module up by calling its shutDown funtion.

	This can be called to shut a module down.
	This does not yet take into account whether the module is still a dependency of another module though and is thus dangerous unless used in the correct order

	\param[in] mod		The module to shut down
	*/
	bool shutdownModule(std::shared_ptr<IModule_API> mod);
	/*!
	\brief Shuts down a module up by calling its shutDown funtion.

	\param[in] mod		The identifier of the module to shut down
	*/
	bool shutdownModule(std::string moduleID);

	/*!
	\brief Returns all modules that fulfill the given type

	This can be used to get a list of all modules that implement the type given.
	A type here is a substring of the . delimited iam field in ModuleInformation.
	This is similar to the java package naming scheme. And can be used to find a module that is valid for a specific dependency.

	\param[in]		The name of the type to implement
	\param[out] 	A hashmap containing all modules assigned to their identifier that fulfill the requirement
	*/
	std::map<std::string, std::shared_ptr<IModule_API>> getModulesOfType(std::string type);


	//
	/*!
	\brief Save the current DependencyGraph to it's source file it was loaded from
	*/
	bool saveDependencyGraph();
	/*!
	\brief Save the current DependencyGraph to it's source file it was loaded from

	\param[in] path		The path the current DependencyGraph should be saved to
	*/
	bool saveDependencyGraph(std::string);

	/*!
	\brief Shuts down all modules in the correct order.

	This will shut down all modules in reverse injection order so that no module will have its dependencies shut down before it is shut down itself.
	This works similar to a breadth first search in that starting from the roots, which are all modules that do not fill any dependencies, all modules of a depth will be shut down first before going deeper.
	*/
	bool shutdown();


private:
	//! Pointer to the ipengine::Core instance
	ipengine::Core *m_core;
	//! Path to the folder from which shared libraries were loaded.
	std::string libFolderPath;
	//! Pointer to the currently active DependencyGraph
	std::shared_ptr<ipdg::DependencyGraph> depgraph;
	//! Hashmap holding all DependencyGraph's loaded
	std::unordered_map<std::shared_ptr<ipdg::DependencyGraph>, std::string> graphs;
	//! Hashmap holding information on whether a DependencyGraph has "uncommitted" changes. Any differences to what was loaded from file.
	std::unordered_map<std::shared_ptr<ipdg::DependencyGraph>, bool> graphHasChanges;
	//! Path to the file of the currently active DependencyGraph
	std::string depgraphpath;
	//! Hashmap keeping all modules that have been loaded. Key is the modules identifier.
	std::map<std::string, std::shared_ptr<IModule_API>> loadedModules;
	//! Hashmap keeping all extensions that have been loaded. Key is the extensions identifier.
	std::map<std::string, std::shared_ptr<IExtension>> loadedExtensions;
	
	std::vector<boost::filesystem::path> dlibFilePaths;

	/*!
	 \brief Recursively injects and starts up a module and all it's dependencies.
	  
	  This function takes a DependencyGraph module node and checks whether a corresponding module has been loaded. All dependencies that are started up are injected. All dependencies not started up yet will be recursively injected.
	  Once all dependencies have been taken care of the second parameter should be "true" to inject the extensions.

	  \param[in] mod	The DependencyGraph module node that should have its dependencies injected.
	  \param[in] doextension	Whether it should inject extensions as described in the DependencyGraph. This should only be set to true \b after all dependencies have been injected.
	*/
	bool recursiveInject(ipdg::Module*, bool = false);


	/*template<typename... Args>
	bool safelyCall(bool (*f)(Args...), std::string errorinfo, Args... args)
	{
		try {
			return (*f)(args...);
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
	}*/
	
	/*!
	\brief Safely calls a modules startUp function and handles all thrown errors through the cores ErrorManager

	\param[in] mod	The module whose startUp should be called.
	\param[in] errorinfo	Additional text to be added to the exceptions report.
	*/
	bool safelyStartup(std::shared_ptr<IModule_API> mod, std::string errorinfo);

	/*!
	\brief Safely calls a modules shutDown function and handles all thrown errors through the cores ErrorManager

	\param[in] mod	The module whose shutDown should be called.
	\param[in] errorinfo	Additional text to be added to the exceptions report.
	*/
	bool safelyShutdown(std::shared_ptr<IModule_API> mod, std::string errorinfo);

	/*!
	\brief Safely calls a modules dependencyUpdated function and handles all thrown errors through the cores ErrorManager

	\param[in] mod	The module whose shutDown should be called.
	\param[in] dep	The identifier of the changed dependency
	\param[in] olddep	The module that filled this dependency before the update
	\param[in] errorinfo	Additional text to be added to the exceptions report.
	*/
	template<typename T>
	bool safelyDependencyUpdated(std::shared_ptr<T> mod, std::string dep, std::shared_ptr<IModule_API> olddep, std::string errorinfo)
	{
		try {
			mod->dependencyUpdated(dep, olddep);
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
		return true;
	}

	std::string getModuleStartupErrorMessage(std::string modname, std::string location)
	{
		return "Failed to start up module " + modname + " at " + location + ". Module error report:\n";
	}
	std::string getModuleShutdownErrorMessage(std::string modname, std::string location)
	{
		return "Failed to shut down module " + modname + " at " + location + ". Module error report:\n";
	}

	std::string getModuleDependencyChangedErrorMessage(std::string modname, std::string dependencyID)
	{
		return modname + "Module failed its dependency update callback at depid" + dependencyID + " at Injector::reassignDependency. Failed module is dependent. Module error report:\n";

	}

	/*!
	\brief Registers all injector commands with the core.

	\param[in] core	The Core instance the commands will be registered with.
	*/
	void registerCommands(ipengine::Core *core);

	void cmd_startupModule(const ipengine::ConsoleParams& params);

	void cmd_shutdownModule(const ipengine::ConsoleParams& params);

	void cmd_loadModule(const ipengine::ConsoleParams& params);

	void cmd_reassignDep(const ipengine::ConsoleParams& params);

	void cmd_getLoadedModules(const ipengine::ConsoleParams& params);

	void cmd_getDependencies(const ipengine::ConsoleParams& params);

	void cmd_getDependencyInfo(const ipengine::ConsoleParams& params);

	void cmd_getModulesOfType(const ipengine::ConsoleParams& params);

	void cmd_removeDependency(const ipengine::ConsoleParams& params);

	void cmd_enableExtension(const ipengine::ConsoleParams& params);

	void cmd_debugswitchgraphics(const ipengine::ConsoleParams& params);
};

#endif

/** @}*/