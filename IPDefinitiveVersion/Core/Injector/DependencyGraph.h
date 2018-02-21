#ifndef _DEPENDENCY_GRAPH_H_
#define _DEPENDENCY_GRAPH_H_

#include <string>
#include <vector>
#include <list>
#include <memory>
#include <algorithm>
#include <iostream>
namespace  DGStuff
{
	struct Dependency;
	struct Extension;
	struct ExtensionPoint
	{
		std::string identifier = "";
		//? do I need a copy/assignment ctor for returning this in functions?
		std::list<Extension> extensions = {};


	};
	struct Module
	{
		friend struct Extension;
		friend struct Dependency;

		Module() {};
		Module(const Module& m) :dependencycounter(m.dependencycounter), extensioncounter(m.extensioncounter), ignore(m.ignore), isExtension(m.isExtension), identifier(m.identifier),
		dependencies(m.dependencies), extensionpoints(m.extensionpoints)
		{

		}

		Module(Module&& other) :dependencycounter(other.dependencycounter), extensioncounter(other.extensioncounter), ignore(other.ignore), isExtension(other.isExtension), identifier(other.identifier)
		{
			dependencies.swap(other.dependencies);
			extensionpoints.swap(other.extensionpoints);
		}

		std::list<std::shared_ptr<Dependency>> dependencies = {};
		std::list<std::shared_ptr<ExtensionPoint>> extensionpoints = {};
		bool isExtension = false;
		bool ignore = true;
		std::string identifier = "";

		const int getDepCounter() { return dependencycounter; }
		const int getExtCounter() { return extensioncounter; }

	protected:
		mutable int dependencycounter = 0;
		mutable int extensioncounter = 0;

		//std::string iname;
	};
	struct Dependency
	{
		std::string identifier = "";
		bool inject = false;


		explicit Dependency(Module *m):inject(false)
		{
			setModule(m);
		}
		Dependency(Module *m, bool inj): inject(inj)
		{
			setModule(m);
		}

		Dependency(const Dependency& other):inject(other.inject), identifier(other.identifier)
		{
			setModule(other.mod);
			/*if (other.mod != nullptr)
			{
				mod = other.mod;
				mod->dependencycounter++;
			}*/
		}

		Dependency(Dependency&& d): identifier(d.identifier), inject(d.inject)
		{
			setModule(d.getModule());
		}

		~Dependency()
		{
			if (mod != nullptr)
			{
				mod->dependencycounter--; //Does this work?
			}
		}

		void setModule(Module *m)
		{
			if (mod != nullptr)
			{
				mod->dependencycounter--;
			}
			if (m != nullptr)
			{
				mod = m;
				mod->dependencycounter++;
			}
		}

		Module* getModule()
		{
			return mod;
		}
	private:
		Module* mod = nullptr;
	};

	struct Extension
	{
		uint32_t priority = 0;
		
		explicit Extension(Module *m) : priority(0)
		{
			setModule(m);
		}

		//? how would I make a copy ctor with this class, set module acts on the input parameter so it can't be const
		Extension(const Extension& e) : priority(e.priority)
		{
			setModule(e.mod);
		}

		Extension(Module *m, uint32_t p) : priority(p)
		{
			setModule(m);
		}

		Extension(Extension&& d) : priority(d.priority)
		{
			setModule(d.getModule());
		}

		~Extension()
		{
			if (mod != nullptr)
			{
				mod->extensioncounter--; //Does this work?
			}
		}

		void setModule(Module *m)
		{
			if (mod != nullptr)
			{
				mod->extensioncounter--;
			}
			if (m != nullptr)
			{
				mod = m;
				mod->extensioncounter++;
			}
		}

		Module* getModule()
		{
			return mod;
		}
	private:
		Module* mod = nullptr;
	};
	
	class DependencyGraph
	{
	private:
		std::list<Module> modules{};

	public:
		DependencyGraph() {}
		DependencyGraph(const DependencyGraph& other) : modules(other.modules) 
		{
			std::cout << "emem"; 
		}

		void add(const Module m)
		{
			modules.push_back(m);
		}
		void remove(const Module &m)
		{
			remove(m.identifier);
		}

		void remove(const std::string identifier)
		{
			modules.remove_if([identifier](DGStuff::Module e)->bool {return e.identifier == identifier; });

		}

		//Return nullptr if not found
		Module* findModule(std::string ident)
		{
			auto mod = std::find_if(modules.begin(), modules.end(), [ident](DGStuff::Module e)->bool {return  e.identifier == ident; });
			if (mod == modules.end())
			{
				return nullptr;
			}
			return &*mod;
		}

		std::list<Module>& getModules()
		{
			return modules;
		}

		const std::list<Module*> getRoots()
		{
			std::list <Module*> roots;
			for (auto& m : modules)
			{
				if (m.getDepCounter() == 0)
					roots.push_back(&m);
			}
			return roots;
		}
	};
}

#endif