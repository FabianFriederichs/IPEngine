#include <vector>
#include <string>
#include <map>
class IModule;

//Have this be modified directly by the injector intead of via setters/getters of module?
struct ModuleInformation
{
	std::map<std::string, IModule*> dependencies;
	//std::vector<std::string> dependencies; //List of the dependency names this module takes. Has to be identical to the string key used by the IModule to hold the IModule reference. Maybe add flags for whethher it's optional?
	std::string iam; //What was this for again?
	std::string identifier; //Modules ID - similiar to java's package names?
	std::string version;
	std::vector<std::string> expoints; //Extension points. Similiar to dependencies ///TODO COMES LATER
	std::string dlibpath; //absolute path to the dynamic library this module comes from
};

class IModule
{
public:
	virtual ModuleInformation* getModuleInfo()=0;
	//virtual bool injectDependency(std::string dependencyID, IModule *dependency) = 0;
};