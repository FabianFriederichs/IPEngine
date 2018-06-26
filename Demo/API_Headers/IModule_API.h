/** \addtogroup Injector
*  @{
*/
/*!
\file IModule_API.h
\brief Defines the interfaces and data structures necessary for dependency injection
*/
#ifndef IMODULE_API_H
#define IMODULE_API_H
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <bitset>
#include <iostream>
#include <IPCore/Core/ICore.h>


namespace DependencyFlags {
	/*!
	\brief Flags to mark a dependency as optional or mandatory for a module to work
	*/
	enum DependencyFlag : size_t
	{
		dp_optional, //!<	Marks a dependency as optional, it doesn't have to be fulfilled for the module to work
		dp_mandatory //!<	Marks a dependency as mandatory, it has to be fulfilled for the module to work
	};
};

class IModule_API;
class Injector;
class IExtension;

/*!
\brief This container holds a modules dependencies and abstracts access to them

The identifier to which dependencies are assigned is the "dependency id". This is defined by the dependent module and used in the DependencyGraph
*/
class DependencyContainer
{
	friend class Injector;
private:
	std::unordered_map<std::string, std::shared_ptr<IModule_API>> dependencies; //!<	Hashmap assigning dependencies to their identifier
protected:

	/*!
	\brief Assigns the given module to the given identifier
	\params[in] dependencyID	the dependency id to which the module is assigned
	\params[in] module			the module that is assigned to the id
	*/
	void assignDependency(const std::string dependencyID, std::shared_ptr<IModule_API> module)
	{
		//assert(size() == 0);
		/*if (!exists(dependencyID))
		{
		dependencies[dependencyID] = module;
		}
		else if (dependencies[dependencyID].second[DependencyFlags::DP_UPDATABLE])
		{
		}*/
		dependencies[dependencyID] = module;
	}

	/*!
	\brief Removes a dependency id entry from the container
	\params[in] dependencyID	the id of the dependency to remove
	*/
	void removeDependency(const std::string dependencyID)
	{
		if (exists(dependencyID))
		{
			dependencies.erase(dependencyID);
		}
	}
public:
	DependencyContainer() :dependencies() {}

	//Copied from std::map
	//template<typename T>
	//T& operator[](const std::string&& dependencyID)
	//{
	//	auto _Where = dependenciesl.overruower_bound(dependencyID);
	//	//Check whether dependency is assigned
	//	if (_Where == dependencies.end() || dependencies._Getcomp()(dependencyID, dependencies._Key(_Where._Mynode())))
	//	{
	//		_Where = dependencies.emplace_hint(_Where, piecewise_construct, _STD forward_as_tuple(_STD move(dependencyID)), tuple<>());
	//	}
	//	
	//	return (dynamic_cast<T>(_Where->second));
	//};

	/*!
	\brief Returns the number of dependencies that are stored in this container
	\params[out] The amount of dependencies stored
	*/
	size_t size()
	{
		return dependencies.size();
	}

	/*!
	\brief Returns the module that fulfills the given dependency. Casting it to the type provided.

	This will return a nullptr if the dependency doesn't exist or the cast is invalid.

	\params[in] T	The type the module should be returned as
	\params[in] dependencyID	The dependency to return
	\params[out] Pointer of type T to the module 
	*/
	template<typename T>
	std::shared_ptr<T> getDep(const std::string dependencyID)
	{
		auto d = dependencies.find(dependencyID);
		if (d == dependencies.end())
		{
			//Doesn't exist case
			return std::shared_ptr<T>(); //nullptr essentially
		}
		std::shared_ptr<T> m = std::dynamic_pointer_cast<T>(d->second); //Assert this?
		return m;
	}


	//Returns true if dependencyID exists as a key
	/*!
	\brief Checks whether the given dependency exists in the container.
	\params[out] True if it exists, false otherwise
	*/
	bool exists(std::string dependencyID)
	{
		//assert(size() == 0);

		return dependencies.count(dependencyID)>0;
	}
};

/*!
\brief Struct holding dependency metadata used for injection rules
*/
struct DependencyInformation
{
	/*DependencyInformation():isMandatory(false), isUpdatable(false), moduleType("")
	{

	}
	DependencyInformation(bool man, bool upd, std::string t) :isMandatory(man), isUpdatable(upd), moduleType(t)
	{

	}

	DependencyInformation(const DependencyInformation& di):isMandatory(di.isMandatory), isUpdatable(di.isUpdatable), moduleType(di.moduleType)
	{

	}

	DependencyInformation &operator=(const DependencyInformation& di) :
		isMandatory()
	{

	}*/
	const bool isMandatory = false; //!< Whether the described dependency is necessary for module function. If this is False then the module \b must be able to function without this dependency
	const bool isUpdatable = false; //!< Whether the described dependency can be updated after the module has been started up for the first time. If this is True then the module \b must handle all necessary state changes in dependencyUpdated
	const std::string moduleType = ""; //!< The type a module has to implement to be eligible for injection.
};

/*!
\brief Parallel to ModuleInformation. Holds Extension metadata.

This is used by extensions to access their dependencies and describe themselves. 
*/
struct ExtensionInformation
{
	//using ExtensionReceptor = std::map<std::string, std::vector<std::shared_ptr<IExtension>>>; //ExP name : [priority]:ExP object 
	DependencyContainer dependencies; //!< Holds the extensions dependencies and makes them accessible
	//std::vector<std::string> dependencies; //List of the dependency names this module takes. Has to be identical to the string key used by the IModule_API to hold the IModule_API reference. Maybe add flags for whethher it's optional?
	//std::string iam; //Maybe make this a string container and have it contain every possible upcast? then you can easily check whether a module can be used as a dependency for X
	std::string identifier; //!< The identifier of this Extension. This is used to identify it after loading from a shared library. It is also used in the DependencyGraph
	std::string version; //!< The version of this Extension. In the future this could be used as further injection requirements
	std::string dlibpath; //!< The absolute file path to the shared library this Extension comes from
	std::map<std::string, DependencyInformation> depinfo; //!< Holds dependency metadata. Used to restrict what modules can be injected. Key is the dependency identifier
};

/*!
\brief The Extension interface. This has to be implemented by all classes acting as an extension

Almost identical to IModule_API. Replaces the startup/shutdown functionality with an active flag.
*/
class IExtension
{
	friend class Injector;
public:
	bool isActive = false; //!< Whether the extension is active. This determines whether execute is called when an ExtensionReceptor is called.
	/*!
	\brief Called by the module through its ExtensionReceptor when an extension point should be executed. 

	Index of argument name is equal to corresponding data index
	\params[in]	A vector of strings containing the names of the provided arguments.
	\params[in] A vector of objects containing the data of the provided arguments.
	*/
	virtual void execute(std::vector<std::string>, std::vector<ipengine::any>&) = 0; 

	/*!
	\brief Returns the extensions metadata
	*/
	virtual ExtensionInformation* getInfo() = 0;

	/*!
	\brief Called by the injector when a dependency has been updated. 

	This is only called when changes happen after initial injection.
	This has to be used to do any clean up or state fixes after a dependency has been changed

	\params[in] depID	The id of the dependency that has been changed
	\params[in] oldDep	Pointer to the module that fulfilled the dependency before the change
	*/
	virtual void dependencyUpdated(std::string depID, std::shared_ptr<IModule_API> oldDep) {};
	//virtual bool injectDependency(std::string dependencyID, IModule_API *dependency) = 0;
protected:
	ipengine::Core* m_core; //!< Pointer to the used Core instance.
};

/*!
\brief This class holds a modules extensions and can be used to execute a specific set of extensions

Extensions are grouped into extension points which are defined by the host module.
The host module does not know anything about what an extension point contains.
It also defines what data is passed to the extensions.

Extensions within an extension point are further sorted by priority. Extensions are executed in order of highest priority (0) to lowest (>0)
*/
class ExtensionReceptor
{
public:
	/*!
	\brief Called by modules when an extension point should be executed.

	Only active extensions are executed
	Index of argument name is equal to corresponding data index
	\params[in] The name of the extension point that should be executed
	\params[in]	A vector of strings containing the names of the provided arguments.
	\params[in] A vector of objects containing the data of the provided arguments.
	*/
	void execute(std::string extensionpointname, std::vector<std::string> argnames, std::vector<ipengine::any> &args)
	{
		for (auto ex : expoints[extensionpointname])
		{
			if (ex->isActive)
			{
				ex->execute(argnames, args);
			}
		}
	}

	/*!
	\brief Returns whether an extension identified by its extension point location is set to active.
	\params[in] extensionpointname	The name identifying the extension point that hosts the extension
	\params[in]	prio	The position of the extension inside the extension point
	*/
	bool isActive(std::string extensionpointname, uint32_t prio=0)
	{
		auto ex = expoints.find(extensionpointname);
		if (ex!=expoints.end())
		{
			return ex->second[prio]->isActive;
		}
	}

	/*!
	\brief Changes an extension identified by its extension point location to active or inactive.
	\params[in] extensionpointname	The name identifying the extension point that hosts the extension
	\params[in]	prio	The position of the extension inside the extension point
	\params[in]	val		The position of the extension inside the extension point
	*/
	void setActive(std::string extensionpointname, uint32_t prio = 0, bool val = true)
	{
		auto ex = expoints.find(extensionpointname);
		if (ex != expoints.end())
		{
			if (ex->second.size() > prio)
				ex->second[prio]->isActive = val;
			else
				ex->second.back()->isActive = val;
		}
	}

	/*!
	\brief Assigns an extension to an extension point with the given priority
	\params[in] pointIdent	The name identifying the extension point that hosts the extension
	\params[in]	prio	The position of the extension inside the extension point
	\params[in]	iexp	Pointer to the IExtension implementing extension
	*/
	void assignExtension(std::string pointIdent, uint32_t prio, std::shared_ptr<IExtension> iexp)
	{
		auto it = expoints[pointIdent].begin();
		if (prio >= expoints[pointIdent].size())
			it = expoints[pointIdent].end();
		else
			it += prio;
		expoints[pointIdent].insert(it, iexp);
	}

	std::map<std::string, std::vector<std::shared_ptr<IExtension>>> expoints; //!< Hashmap assigning extension points (vector of IExtension pointers) to extension point identifiers/names
};


/*!
\brief Parallel to ExtensionInformation. Holds module metadata.

This is used by modules to access their dependencies and describe themselves.
*/
struct ModuleInformation
{
	//using ExtensionReceptor = std::map<std::string, std::vector<std::shared_ptr<IExtension>>>; //ExP name : [priority]:ExP object 
	DependencyContainer dependencies; //!< Holds the modules dependencies and makes them accessible
	//std::vector<std::string> dependencies; //List of the dependency names this module takes. Has to be identical to the string key used by the IModule_API to hold the IModule_API reference. Maybe add flags for whethher it's optional?
	std::string iam; //!< A string containing . delimited substrings listing all module APIs this module implements. Used to check whether a module implements a type necessary for a dependency
	std::string identifier; //!< The identifier of this module. This is used to identify it after loading from a shared library. It is also used in the DependencyGraph
	std::string version; //!< The version of this module. In the future this could be used as further injection requirements
	ExtensionReceptor expoints; //Extension points. Similiar to dependencies ///TODO COMES LATER
	std::string dlibpath; //!< The absolute file path to the shared library this module comes from
	std::map<std::string, DependencyInformation> depinfo; //!< Holds dependency metadata. Used to restrict what modules can be injected. Key is the dependency identifier

};


/*!
\brief The Module interface. This has to be implemented by all classes acting as a module.

All modules need to implement this interface to work with the injector. 
It is also necessary to export an instance of your module that can be loaded.
The symbol has to be named \b module.

Example:
\code{.cpp}
extern "C" __declspec(dllexport) GraphicsModule module;
GraphicsModule module;
\endcode

Almost identical to IExtension.
*/
class IModule_API
{
	friend class Injector;
public:
	/*!
	\brief Returns the modules metadata
	*/
	virtual ModuleInformation* getModuleInfo() = 0;
					
protected:
	ipengine::Core* m_core; //!< Pointer to the used Core instance.

	//Returns true if startup is successful. This is called after dependencies have been injected. Handle all the initialization necessary. Probably should replace this with error code memes. 
	/*!
	\brief This is called by the injector after all dependencies have been successfully injected. 
	Returns true if startup is successfull
	*/
	bool startUp()
	{
		isStartUp = _startup();
		return isStartUp;
	}

	/*!
	\brief This is called by the injector if the module should be shut down
	Returns true if shut down is successfull
	*/
	bool shutDown()
	{
		isStartUp = !_shutdown();
		return isStartUp;
	}

	//Should be overriden by modules that have dependencies that can be updated at runtime.
	/*!
	\brief Called by the injector when a dependency has been updated after initial start up.

	This has to be used to do any clean up or state fixes after a dependency has been changed.
	If a dependency is marked as \b updateable then this has to be implemented if the change breaks state or clean up has to be performed in the module previously fulfilling the dependency.

	\params[in] depID	The id of the dependency that has been changed
	\params[in] oldDep	Pointer to the module that fulfilled the dependency before the change
	*/
	virtual void dependencyUpdated(std::string depID, std::shared_ptr<IModule_API> oldDep) {};

	//virtual bool injectDependency(std::string dependencyID, IModule_API *dependency) = 0;
private:
	/*!
	\brief Module initialization code goes in here.
	Returns true if successfull. Module state has to be valid in that case
	*/
	virtual bool _startup() = 0;
	/*!
	\brief Module shut down code goes in here.
	Returns true if successfull. Module state has to be valid in either case and clean up on dependencies should be performed.
	*/
	virtual bool _shutdown() = 0;
	bool isStartUp = false;//!< True if the module has been successfully started up
};



#endif