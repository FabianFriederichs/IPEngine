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
		std::vector<Module*> roots;
		std::vector<Module> modules;
		/*template<typename F>
		static Module* recursiveApply(const Module *m, F &lambda)
		{
			if (m->dependencies.size() > 0)
			{
				for (auto m : m->dependencies)
				{
					recursiveApply(m.second, lambda);
				}
			}
			lambda(m);
		}*/
	public:
		void addModule(Module m) {modules.push_back(m);}
		void addRoot(Module *m) { roots.push_back(m); }
		std::vector<Module> *getModules(){ return &modules; };
		std::vector<Module*> *getRoots(){ return &roots; }
		/*template<typename F>
		static void forEachReverse(const Module *m, F &lambda)
		{
			recursiveApply(m, lambda);
		}*/
	};
 }
 