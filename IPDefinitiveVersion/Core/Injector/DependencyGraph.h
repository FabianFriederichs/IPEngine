#include <string>
#include <vector>
#include <map>
namespace  DGStuff
{
	struct Extension
	{
		std::string identifier;
		bool inject;
	};

	struct ExtensionPoint
	{
		std::string extensionpointidentifier;

		std::map<uint32_t, Extension*> extensions;
	};

	struct Module
	{
		std::map<std::string, Module*> dependencies;
		std::map<std::string, bool> dontInject;
		std::vector<ExtensionPoint> extensions;
		std::string identifier;
		std::string iname;
	};

	class DependencyGraph
	{
	private:
		std::vector<Module*> roots;
		std::vector<Module> modules;
		std::vector<Extension> extensions;
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
		void addModule(Module m) { modules.push_back(m); }
		void addExtension(Extension e) { extensions.push_back(e); }
		void addRoot(Module *m) { roots.push_back(m); }
		std::vector<Extension>* getExtensions(){ return &extensions; }
		std::vector<Module> *getModules() { return &modules; };
		std::vector<Module*> *getRoots() { return &roots; }
		/*template<typename F>
		static void forEachReverse(const Module *m, F &lambda)
		{
		recursiveApply(m, lambda);
		}*/
	};
}
