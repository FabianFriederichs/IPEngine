#include "XMLParser.h"
#include <iostream>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/dll/import.hpp>
#include <boost/dll/alias.hpp>
#include <boost/function/function_base.hpp>

ipdg::Module XMLParser::ptreeToModule(const boost::property_tree::ptree * node)
{
	ipdg::Module tmod;

	//properties
	//ignore - bool
	//identifier - string
	//isExPoint - bool
	//dependencies - dependency nodes
	//ExtensionPoints - Point nodes
	tmod.ignore = node->get("ignore", false);
	tmod.isExtension = node->get("isExPoint", false);
	tmod.identifier = node->get<std::string>("identifier","");
	return tmod;
}

std::shared_ptr<ipdg::Dependency> XMLParser::ptreeToDep(const boost::property_tree::ptree * node, std::list<ipdg::Module>& mods)
{

	//properties
	//inject - bool
	//identifier - string
	//moduleID - string
	auto inj = node->get<bool>("inject", false);
	auto ident = node->get<std::string>("identifier", "");
	auto modid = node->get<std::string>("moduleID", "");
	auto mod = std::find_if(mods.begin(), mods.end(), [modid](ipdg::Module e)->bool {return  e.identifier == modid; });
	if (mod!=mods.end())
	{
		auto d = std::make_shared<ipdg::Dependency>(&(*mod), inj);
		d->identifier = ident;
		return d;
	}
	return std::shared_ptr<ipdg::Dependency>(nullptr); //returns nullptr
}

std::shared_ptr<ipdg::ExtensionPoint> XMLParser::ptreeToExP(const boost::property_tree::ptree * node, std::list<ipdg::Module>& mods)
{
	auto tpoint = std::make_shared<ipdg::ExtensionPoint>();
	//properties
	//identifier - string
	//Extensions - Extension node
		//extensionname - string
		//priority - uint
	
	tpoint->identifier = node->get<std::string>("identifier", "");

	for (auto child : node->get_child("Extensions"))
	{
		auto modid = child.second.get<std::string>("extensionname", "");
		auto mod = std::find_if(mods.begin(), mods.end(), [modid](ipdg::Module e)->bool {return  e.identifier == modid; });
		if (mod != mods.end()) //! error mesage or something when skipping nodes because it's not keeping format
		{
			auto e = ipdg::Extension(&(*mod), child.second.get<uint32_t>("priority", 0));
			tpoint->extensions.push_back(e); //?do I need to move or is my assign copy ctor ok? or is it a different ctor I need?
		}
	}

	return tpoint;
}

void XMLParser::ptreeModuleDep(const boost::property_tree::ptree * node, ipdg::DependencyGraph& graph)
{
	//get module
	auto ident = node->get<std::string>("identifier", "");
	auto mod = graph.findModule(ident);
	if (mod == nullptr)
	{
		return; //Module not found in graph
	}
	//iterate dependencies
	if (node->count("dependencies"))
	{
		for (auto child : node->get_child("dependencies"))
		{
			auto d = ptreeToDep(&child.second, graph.getModules());
			if (d == nullptr) //unsuccessful dependency parse
				continue;
			mod->dependencies.push_back(d);
		}
	}

	//iterate extensions
	if (node->count("ExtensionPoints"))
	{
		for (auto child : node->get_child("ExtensionPoints"))
		{
			auto e = ptreeToExP(&child.second, graph.getModules());
			//if (e == nullptr) //unsuccessful exp parse
			//	continue;
			mod->extensionpoints.push_back(e);
		}
	}
}

XMLParser::XMLParser()
{
}


XMLParser::~XMLParser()
{
}

XMLParser::pDepGraph XMLParser::parse(std::string path)
{
	pDepGraph dgraph = std::make_shared<ipdg::DependencyGraph>();
	boost::property_tree::ptree tree;
	try {
		boost::property_tree::read_xml(path, tree, boost::property_tree::xml_parser::no_comments);//Check out what tree has in it if parsing fails
	}
	catch (boost::property_tree::xml_parser_error& ex)
	{
		std::throw_with_nested(std::exception("Failed to parse dependency tree xml in XMLParser::parse"));
	}
	//create module for each module node in ptree
	//auto n = tree.get_child("DependencyGraph.Modules");
	for (auto node : tree.get_child("DependencyGraph.Modules"))
	{
		auto m = ptreeToModule(&node.second);
		if (m.identifier != "")
		{
			dgraph->add(m);
			//! error mesage or something when skipping nodes because it's not keeping format
		}
	}

	//create dependencies and extensions for every module
	for (auto node : tree.get_child("DependencyGraph.Modules"))
	{
		ptreeModuleDep(&node.second, *dgraph);
	}
	_lastResult = ParseResult::READING_SUCCESS;
	return dgraph; //? is a std::move sensible here?
}

XMLParser::ParseResult XMLParser::write(ipdg::DependencyGraph& graph, std::string path)
{
	//TODO
	boost::property_tree::ptree tree;
	//set up base
	auto& modnode = tree.add("DependencyGraph", "").add("Modules","");
	auto modules = graph.getModules();
	for (auto& mod : modules)
	{
		auto& tmod = modnode.add("Module", "");
		tmod.add("identifier", mod.identifier);
		tmod.add("ignore", mod.ignore);
		tmod.add("isExPoint", mod.isExtension);
		if (mod.dependencies.size() > 0)
		{
			auto& tdeps = tmod.add("dependencies", "");
			for (auto d : mod.dependencies)
			{
				auto& td = tdeps.add("dependency", "");
				td.add("inject", d->inject);
				td.add("identifier", d->identifier);
				td.add("moduleID", d->getModule()->identifier);
			}

		}

		if (mod.extensionpoints.size() > 0)
		{
			auto& texps = tmod.add("ExtensionPoints", "");
			for (auto ep : mod.extensionpoints)
			{
				auto& tep = texps.add("Point", "");
				tep.add("identifier", ep->identifier);
				if (ep->extensions.size() > 0)
				{
					auto& tes = tep.add("Extensions", "");
					for (auto& e : ep->extensions)
					{
						auto& te = tes.add("Extension", "");
						te.add("extensionname", e.getModule()->identifier);
						te.add("priority", e.priority);
					}
				}
			}
		}
		
	}
	
	try {
		boost::property_tree::write_xml(path, tree);
	}
	catch (boost::property_tree::xml_parser_error ex)
	{
		////todo handle stuff
		//_lastResult = ParseResult::WRITING_FAILED;
		//return getResult();
		std::throw_with_nested(std::exception("Failed to write property tree to xml file in XMLParser::write"));
	}
	return ParseResult::WRITING_SUCCESS;
}

XMLParser::ParseResult XMLParser::getResult()
{
	return _lastResult;
}
