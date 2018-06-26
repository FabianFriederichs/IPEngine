#ifndef _XMLPARSER_H_
#define _XMLPARSER_H_

#include "DependencyParser.h"
#include <boost/property_tree/xml_parser.hpp>
#include <memory>
#pragma once
class XMLParser : DependencyParser
{
public:
	
private:
	ipdg::Module ptreeToModule(const boost::property_tree::ptree *);
	std::shared_ptr<ipdg::Dependency> ptreeToDep(const boost::property_tree::ptree *, std::list<ipdg::Module>&);
	std::shared_ptr<ipdg::ExtensionPoint> ptreeToExP(const boost::property_tree::ptree *, std::list<ipdg::Module>&);
	void ptreeModuleDep(const boost::property_tree::ptree *, ipdg::DependencyGraph &);

	ParseResult _lastResult;
public:
	XMLParser();
	~XMLParser();
		
	// Inherited via DependencyParser
	virtual pDepGraph parse(std::string) override;
	virtual ParseResult write(ipdg::DependencyGraph&, std::string) override;

	// Inherited via DependencyParser
	virtual ParseResult getResult() override;
};

#endif // !_XMLPARSER_H_