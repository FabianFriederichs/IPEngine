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
		std::vector<const Module*> modules;
	public:
		void addModule(const Module *m) {modules.push_back(m);}
		void addRoot(const Module &m) { roots.push_back(m); }
		std::vector<const Module*> *getModules(){ return &modules; };
	};
 }
 