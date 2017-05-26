#ifndef IMODULE_API_H
#define IMODULE_API_H
#include <vector>
#include <string>
#include <map>
class IModule_API;

//Have this be modified directly by the injector intead of via setters/getters of module?
struct ModuleInformation
{
	std::map<std::string, IModule_API*> dependencies;
	//std::vector<std::string> dependencies; //List of the dependency names this module takes. Has to be identical to the string key used by the IModule_API to hold the IModule_API reference. Maybe add flags for whethher it's optional?
	std::string iam; //What was this for again?
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
	//virtual bool injectDependency(std::string dependencyID, IModule_API *dependency) = 0;
};

#endif