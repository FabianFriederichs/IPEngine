/** \addtogroup Injector
*  @{
*/
/*!
\file DependencyGraph.h
\brief Defines the dependency graph used for describing module dependencies in the injector
*/

#ifndef _DEPENDENCY_GRAPH_H_
#define _DEPENDENCY_GRAPH_H_

#include <string>
#include <vector>
#include <list>
#include <memory>
#include <algorithm>
#include <iostream>

/*!
\brief Collection of dependency graph data structures used by the injector to represent dependencies.

The format used by the parser used by the injector right now is xml and is as follows:

\code{.unparsed}
<DependencyGraph>
	<Modules>
		<Module>
			<ignore>false</ignore>
			<identifier>SDLWindowManager</identifier>
		</Module>
		<Module>
			<ignore>false</ignore>
			<identifier>SimpleContentModule</identifier>
		</Module>
		<Module>
			<ignore>false</ignore>
			<identifier>PhysicsDataParser</identifier>
			<isExPoint>true</isExPoint>
		</Module>
		<Module>
			<ignore>false</ignore>
			<identifier>OpenGLGraphicsModule</identifier>
			<dependencies>
				<dependency>
					<inject>true</inject>
					<identifier>SCM</identifier>
					<moduleID>SimpleContentModule</moduleID>
				</dependency>
				<dependency>
					<inject>true</inject>
					<identifier>WindowManager</identifier>
					<moduleID>SDLWindowManager</moduleID>
				</dependency>
			</dependencies>
		</Module>
		<Module>
			<identifier>SimpleSceneModule</identifier>
			<dependencies>
				<dependency>
					<inject>true</inject>
					<identifier>contentmodule</identifier>
					<moduleID>SimpleContentModule</moduleID>
				</dependency>
			</dependencies>
			<ExtensionPoints>
				<Point>
					<identifier>ExtendedEntity</identifier>
					<Extensions>
						<Extension>
							<extensionname>PhysicsDataParser</extensionname>
							<priority>0</priority>
						</Extension>
					</Extensions>
				</Point>
			</ExtensionPoints>
		</Module>
	</Modules>
</DependencyGraph>
\endcode

*/
namespace  ipdg
{
	struct Dependency;
	struct Extension;

	/*!
	\brief Represents an ExtensionPoint as used in ExtensionReceptor
	*/
	struct ExtensionPoint
	{
		std::string identifier = ""; //!< String identifier used as the extension points name
		//? do I need a copy/assignment ctor for returning this in functions?
		std::list<Extension> extensions = {}; //!< List of Extension registered in this ExtensionPoint


	};

	/*!
	\brief Represents a module in the dependency graph
	Contains a list of dependencies and extension points as well as some meta data

	A module is a node inside the dependency graph. 
	A module can fill the dependency of another module. In this context such a module is called a dependency.
	A module can be an extension. 

	A module keeps track of how many dependencies it fills. This is used to determine the depth of a node.

	*/
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

		std::list<std::shared_ptr<Dependency>> dependencies = {}; //!< Dependencies
		std::list<std::shared_ptr<ExtensionPoint>> extensionpoints = {}; //!< ExtensionPoints
		bool isExtension = false; //!< Whether this module represents an extension or a module
		bool ignore = true; //!< Whether this node should be ignored by the injector
		std::string identifier = ""; //!< The identifier of this module. Identical to the identifier in ModuleInformation or ExtensionInformation

		const int getDepCounter() { return dependencycounter; }
		const int getExtCounter() { return extensioncounter; }

	protected:
		mutable int dependencycounter = 0; //!< How many dependencies this module fills
		mutable int extensioncounter = 0; //!< how many extension points this module is part of

		//std::string iname;
	};

	/*!
	\brief Represents a dependency of a module in the dependency graph
	Contains a reference to the Module this dependency is filled by

	The identifier is the name of this dependency as defined by the dependent module. Not the identifier of the module that fills this dependency.
	*/
	struct Dependency
	{
		std::string identifier = ""; //!< The dependency identifier as described by the dependent module
		bool inject = false; //!< Whether this dependency should be injected


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

		/*!
		\brief Sets the module that fills this dependency. This adjusts the modules dependency counter to keep track of node depth
		*/
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

		/*!
		\brief Returns the pointer of the module that fills this dependency
		*/
		Module* getModule()
		{
			return mod;
		}
	private:
		Module* mod = nullptr; //!< Pointer to the module that fills this dependency
	};

	/*!
	\brief Represents an extension inside of a modules extension point in the dependency graph
	Contains a reference to the Module this extension is represented by
	*/
	struct Extension
	{
		uint32_t priority = 0; //!< The priority this extension should take
		
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

		/*!
		\brief Sets the module that represents this extension. This adjusts the modules extension counter to keep track of node depth
		*/
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

		/*!
		\brief Returns the pointer of the module that represents this extension
		*/
		Module* getModule()
		{
			return mod;
		}
	private:
		Module* mod = nullptr; //!< Pointer to the module that fills this dependency
	};
	
	/*!
	\brief Encapsulates a list of Modules that represent the dependency graph. Provides some convenience functions

	All modules that have a depth/dependency counter of 0 are considered roots. These modules do not act as dependency of any other module.
	The depth is used to determine the injection order. All modules with the highest depth and fewest dependencies should be started up/have their dependencies injected first, as this makes injection less error-prone.
	*/
	class DependencyGraph
	{
	private:
		std::list<Module> modules{}; //!< List of all modules. Not just roots

	public:
		DependencyGraph() {}
		DependencyGraph(const DependencyGraph& other) : modules(other.modules)
		{
		}

		DependencyGraph(DependencyGraph&& other)
		{
			modules.swap(other.modules);
		}

		/*!
		\brief Creates a deep copy of the dependency graph. 
		*/
		DependencyGraph deepCopy()
		{
			DependencyGraph g;
			for (auto m : modules)
			{
				Module tm;
				tm.identifier = m.identifier;
				tm.ignore = m.ignore;
				tm.isExtension = m.isExtension;
				g.modules.push_back(tm);
			}

			for (auto m : modules)
			{
				auto dependent = g.findModule(m.identifier);
				for (auto& deps : m.dependencies)
				{
					auto dependency = g.findModule(deps->getModule()->identifier);
					auto shdep = std::make_shared<Dependency>(dependency, deps->inject);
					shdep->identifier = deps->identifier;
					dependent->dependencies.push_back(shdep);
				}
				for (auto& exp : m.extensionpoints)
				{
					auto tpoint = std::make_shared<ExtensionPoint>();
					tpoint->identifier = exp->identifier;
					for (auto& ext : exp->extensions)
					{
						auto extensionmod = g.findModule(ext.getModule()->identifier);
						tpoint->extensions.push_back({ extensionmod, ext.priority });
					}
					dependent->extensionpoints.push_back(tpoint);
				}
			}
			return g;
		}

		/*!
		\brief Adds a module to the module list
		*/
		void add(const Module m)
		{
			modules.push_back(m);
		}

		/*!
		\brief Removes a module from the module list
		*/
		void remove(const Module &m)
		{
			remove(m.identifier);
		}

		/*!
		\brief Remove a module from the module list by module identifier
		*/
		void remove(const std::string identifier)
		{
			modules.remove_if([identifier](ipdg::Module e)->bool {return e.identifier == identifier; });

		}

		//Return nullptr if not found
		/*!
		\brief Returns the pointer of the module that has the given identifier
		\params[in] ident	String identifier of the module to find
		*/
		Module* findModule(std::string ident)
		{
			auto mod = std::find_if(modules.begin(), modules.end(), [ident](ipdg::Module e)->bool {return  e.identifier == ident; });
			if (mod == modules.end())
			{
				return nullptr;
			}
			return &*mod;
		}

		/*!
		\brief Updates the dependencies of a module. Used to update the graph with changes applied by the injector

		\params[in] dependentIdent Identifier of the module whose dependency is changed
		\params[in] dependencyIdent Identifier of the dependency that is changed
		\params[in] replacingModule Identifier of the module that fills the dependency now
		*/
		void changeDependency(std::string dependentIdent, std::string dependencyIdent, std::string replacingModule)
		{
			auto dependent = findModule(dependentIdent);
			auto replacer = findModule(replacingModule);
			if (dependent != nullptr && replacer != nullptr)
			{
				auto dependency = std::find_if(dependent->dependencies.begin(), dependent->dependencies.end(), [dependencyIdent](std::shared_ptr<ipdg::Dependency> e)->bool {return e->identifier == dependencyIdent; });
				if (dependency != dependent->dependencies.end())
				{
					(*dependency)->setModule(replacer);
				}
			}
		}

		/*!
		\brief Returns a list of all modules
		*/
		std::list<Module>& getModules()
		{
			return modules;
		}

		/*!
		\brief Returns a list of all roots in the graph
		*/
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