#include <string>
#include <vector>
#include <map>
namespace  DGStuff
{
	struct ExtensionPoint
	{
		std::string extensionpointidentifier;
		int position;
		std::string extensionidentifier;
	};

	struct Module
	{
		std::map<std::string, Module*> dependencies;
		std::vector<ExtensionPoint> extensions;
		std::string identifier;
		std::string iname;
	};
	 
	class DependencyGraph
	{
	private:
		std::vector<Module> roots;
	public:
		void addModule(const Module &m) {roots.push_back(m);}
	};
 }
 