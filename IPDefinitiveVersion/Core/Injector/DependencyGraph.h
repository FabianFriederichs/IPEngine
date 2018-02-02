#ifndef _DEPENDENCY_GRAPH_H_
#define _DEPENDENCY_GRAPH_H_

#include <string>
#include <vector>
#include <list>
#include <memory>
namespace  DGStuff
{
	struct Dependency
	{
		std::string identifier;
		bool inject;


		Dependency(Module *m)
		{
			setModule(m);
			inject = false;
		}
		Dependency(Module *m, bool inj)
		{
			setModule(m);
			inject = inj;
		}

		~Dependency()
		{
			if (mod.get() != nullptr)
			{
				mod->dependencycounter--; //Does this work?
			}
		}

		void setModule(Module *m)
		{
			if (mod.get() != nullptr)
			{
				mod->dependencycounter--;
			}
			mod = std::shared_ptr<Module>(m);
			mod->dependencycounter++;
		}

		std::shared_ptr<Module> getModule()
		{
			return mod;
		}
	private:
		std::shared_ptr<Module> mod;
	};

	struct Extension
	{
		std::string identifier;
		//bool inject;
		uint32_t priority;
		
		Extension(Module *m)
		{
			setModule(m);
			priority = 0;
		}
		Extension(Module *m, uint32_t p)
		{
			setModule(m);
			priority = p;
		}

		~Extension()
		{
			if (mod.get() != nullptr)
			{
				mod->extensioncounter--; //Does this work?
			}
		}

		void setModule(Module *m)
		{
			if (mod.get() != nullptr)
			{
				mod->extensioncounter--;
			}
			mod = std::shared_ptr<Module>(m);
			mod->extensioncounter++;
		}

		std::shared_ptr<Module> getModule()
		{
			return mod;
		}
	private:
		std::shared_ptr<Module> mod;
	};
	
	struct ExtensionPoint;
	struct Dependency;
	struct Module
	{
		friend class Extension;
		friend class Dependency;

		std::list<std::shared_ptr<Dependency>> dependencies;
		std::list<std::shared_ptr<ExtensionPoint>> extensionpoints;
		bool isExtension;
		bool ignore;
		std::string identifier;

		const int getDepCounter() { return dependencycounter; }
		const int getExtCounter() { return extensioncounter; }
	
	protected:
		int dependencycounter = 0;
		int extensioncounter = 0;

		//std::string iname;
	};

	struct ExtensionPoint
	{
		std::string identifier;

		std::list<Extension> extensions;
	};

	class DependencyGraph
	{
	private:
		std::list<Module> modules;
		//std::vector<Module*> extensions;
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
		//void addModule(Module m) { modules.push_back(m); }
		////void addExtension(Extension e) { extensions.push_back(e); }
		//void addRoot(Module *m) { roots.push_back(m); }
		////std::vector<Extension>* getExtensions(){ return &extensions; }
		//std::list<Module> *getModules() { return &modules; };
		//std::list<Module*> *getRoots() { return &roots; }
		/*template<typename F>
		static void forEachReverse(const Module *m, F &lambda)
		{
		recursiveApply(m, lambda);
		}*/

		void add(const Module m)
		{
			modules.push_back(m);
		}
		void remove(const Module &m)
		{
			modules.remove(m);
		}

		const std::list<Module>& getModules()
		{
			return modules;
		}
		const std::list<std::shared_ptr<Module>> getRoots()
		{
			std::list<std::shared_ptr<Module>> roots;
			for (auto m : modules)
			{
				if (m.getDepCounter() == 0)
					roots.push_back(std::shared_ptr<Module>(&m));
			}
			return roots;
		}
	};
}

#endif