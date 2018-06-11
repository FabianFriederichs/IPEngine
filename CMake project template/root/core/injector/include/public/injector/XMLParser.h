#ifndef _XMLPARSER_H_
#define _XMLPARSER_H_

#include <injector/DependencyParser.h>
#include <boost/property_tree/xml_parser.hpp>
#include <memory>
#pragma once
class XMLParser : DependencyParser
{
public:
	
private:
	DGStuff::Module ptreeToModule(const boost::property_tree::ptree *);
	std::shared_ptr<DGStuff::Dependency> ptreeToDep(const boost::property_tree::ptree *, std::list<DGStuff::Module>&);
	std::shared_ptr<DGStuff::ExtensionPoint> ptreeToExP(const boost::property_tree::ptree *, std::list<DGStuff::Module>&);
	void ptreeModuleDep(const boost::property_tree::ptree *, DGStuff::DependencyGraph &);

	ParseResult _lastResult;
public:
	XMLParser();
	~XMLParser();
		
	// Inherited via DependencyParser
	virtual pDepGraph parse(std::string) override;
	virtual ParseResult write(DGStuff::DependencyGraph&, std::string) override;

	// Inherited via DependencyParser
	virtual ParseResult getResult() override;
};

#endif // !_XMLPARSER_H_