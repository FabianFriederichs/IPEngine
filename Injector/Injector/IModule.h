#include <vector>
#include <string>

struct ModuleInformation
{
	vector<string> dependencies;
	string iam;
	string identifier;
	string version;
	vector<string> expoints;
	string dlibpath;
}

class IModule
{
public:
	ModuleInformation getModuleInfo()=0;
	
}

