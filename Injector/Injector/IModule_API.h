#ifndef IMODULE_API_H
#define IMODULE_API_H
#include <vector>
#include <string>
#include <map>
#include <boost\smart_ptr.hpp>
#include <bitset>

namespace DependencyFlags{
	enum DependencyFlag : size_t
	{
		DP_OPTIONAL,
		DP_UPDATABLE
	};
};

class IModule_API;

class DependencyContainer
{
private:
	using depPair = std::pair<boost::shared_ptr<IModule_API>, std::bitset<2>>;
	std::map<std::string, depPair> dependencies;
public:
	DependencyContainer(){};

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

	

	void assignDependency(const std::string dependencyID, boost::shared_ptr<IModule_API> module, std::bitset<2> flags = 0)
	{
		if (!exists(dependencyID))
		{
			dependencies[dependencyID] = { module, flags };
		}
		else if (dependencies[dependencyID].second[DependencyFlags::DP_UPDATABLE])
		{
			dependencies[dependencyID].first = module;
		}
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
		boost::shared_ptr<T> m = boost::dynamic_pointer_cast<T>(d->second.first); //Assert this?
		return m;
	}


	//Returns true if dependencyID exists as a key
	bool exists(std::string dependencyID)
	{
		return dependencies.find(dependencyID) != dependencies.end();
	}
};

	

struct ModuleInformation
{
	DependencyContainer dependencies;
	//std::vector<std::string> dependencies; //List of the dependency names this module takes. Has to be identical to the string key used by the IModule_API to hold the IModule_API reference. Maybe add flags for whethher it's optional?
	std::string iam; //Maybe make this a string container and have it contain every possible upcast? then you can easily check whether a module can be used as a dependency for X
	std::string identifier; //Modules ID - similiar to java's package names?
	std::string version;
	std::vector<std::string> expoints; //Extension points. Similiar to dependencies ///TODO COMES LATER
	std::string dlibpath; //absolute path to the dynamic library this module comes from
};

class IModule_API
{
public:
	virtual ModuleInformation* getModuleInfo()=0;
	virtual bool startUp() = 0; //Returns true if startup is successful. This is called after dependencies have been injected. Handle all the initialization necessary. Probably should replace this with error code memes. 
	//Should be overriden by modules that have dependencies that can be updated at runtime.
	virtual void dependencyUpdated(std::string depID){};
	//virtual bool injectDependency(std::string dependencyID, IModule_API *dependency) = 0;
};

#endif