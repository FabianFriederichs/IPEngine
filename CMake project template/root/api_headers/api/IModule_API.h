#ifndef IMODULE_API_H
#define IMODULE_API_H
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <boost/smart_ptr.hpp>
#include <bitset>
#include <iostream>
#include <core/util/any.h>
#include <core/core_types.h>
#include <core/core.h>

namespace DependencyFlags {
	enum DependencyFlag : size_t
	{
		dp_optional,
		dp_mandatory
	};
};

class IModule_API;
namespace ipengine { class Core; }
class Injector;
class IExtension;
class DependencyContainer
{
	friend class Injector;
private:
	std::unordered_map<std::string, boost::shared_ptr<IModule_API>> dependencies;
protected:
	void assignDependency(const std::string dependencyID, boost::shared_ptr<IModule_API> module)
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

	size_t size()
	{
		return dependencies.size();
	}

	template<typename T>
	boost::shared_ptr<T> getDep(const std::string dependencyID)
	{
		auto d = dependencies.find(dependencyID);
		if (d == dependencies.end())
		{
			//Doesn't exist case
			return boost::shared_ptr<T>(); //nullptr essentially
		}
		boost::shared_ptr<T> m = boost::dynamic_pointer_cast<T>(d->second); //Assert this?
		return m;
	}


	//Returns true if dependencyID exists as a key
	bool exists(std::string dependencyID)
	{
		//assert(size() == 0);

		return dependencies.lower_bound(dependencyID) != dependencies.end();
	}
};

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
	const bool isMandatory = false;
	const bool isUpdatable = false;
	const std::string moduleType = "";
};

struct ExtensionInformation
{
	//using ExtensionReceptor = std::map<std::string, std::vector<boost::shared_ptr<IExtension>>>; //ExP name : [priority]:ExP object 
	DependencyContainer dependencies;
	//std::vector<std::string> dependencies; //List of the dependency names this module takes. Has to be identical to the string key used by the IModule_API to hold the IModule_API reference. Maybe add flags for whethher it's optional?
	//std::string iam; //Maybe make this a string container and have it contain every possible upcast? then you can easily check whether a module can be used as a dependency for X
	std::string identifier; //Modules ID - similiar to java's package names?
	std::string version;
	std::string dlibpath; //absolute path to the dynamic library this module comes from
	std::map<std::string, DependencyInformation> depinfo; //Key is the dependency identifier

};

class IExtension
{
	friend class Injector;
public:
	bool isActive = false;
	virtual void execute(std::vector<std::string>, std::vector<ipengine::any>&) = 0;

	virtual ExtensionInformation* getInfo() = 0;
	virtual void dependencyUpdated(std::string depID) {};
	//virtual bool injectDependency(std::string dependencyID, IModule_API *dependency) = 0;
protected:
	ipengine::Core* m_core;
};

class ExtensionReceptor
{
public:
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

	bool isActive(std::string extensionpointname, uint32_t prio=0)
	{
		auto ex = expoints.find(extensionpointname);
		if (ex!=expoints.end())
		{
			return ex->second[prio]->isActive;
		}
	}

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

	void assignExtension(std::string pointIdent, uint32_t prio, boost::shared_ptr<IExtension> iexp)
	{
		auto it = expoints[pointIdent].begin();
		if (prio >= expoints[pointIdent].size())
			it = expoints[pointIdent].end();
		else
			it += prio;
		expoints[pointIdent].insert(it, iexp);
	}

	std::map<std::string, std::vector<boost::shared_ptr<IExtension>>> expoints;
};

struct ModuleInformation
{
	//using ExtensionReceptor = std::map<std::string, std::vector<boost::shared_ptr<IExtension>>>; //ExP name : [priority]:ExP object 
	DependencyContainer dependencies;
	//std::vector<std::string> dependencies; //List of the dependency names this module takes. Has to be identical to the string key used by the IModule_API to hold the IModule_API reference. Maybe add flags for whethher it's optional?
	std::string iam; //Maybe make this a string container and have it contain every possible upcast? then you can easily check whether a module can be used as a dependency for X
	std::string identifier; //Modules ID - similiar to java's package names?
	std::string version;
	ExtensionReceptor expoints; //Extension points. Similiar to dependencies ///TODO COMES LATER
	std::string dlibpath; //absolute path to the dynamic library this module comes from
	std::map<std::string, DependencyInformation> depinfo; //Key is the dependency identifier

};



class IModule_API
{
	friend class Injector;
public:
	virtual ModuleInformation* getModuleInfo() = 0;
					
protected:
	ipengine::Core* m_core;

	//Returns true if startup is successful. This is called after dependencies have been injected. Handle all the initialization necessary. Probably should replace this with error code memes. 
	bool startUp()
	{
		isStartUp = _startup();
		return isStartUp;
	}

	bool shutDown()
	{
		isStartUp = _shutdown();
		return isStartUp;
	}

	//Should be overriden by modules that have dependencies that can be updated at runtime.
	virtual void dependencyUpdated(std::string depID) {};

	//virtual bool injectDependency(std::string dependencyID, IModule_API *dependency) = 0;
private:
	virtual bool _startup() = 0;
	virtual bool _shutdown() = 0;
	bool isStartUp = false;
};



#endif