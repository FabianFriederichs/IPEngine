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

public:
	XMLParser();
	~XMLParser();
		
	// Inherited via DependencyParser
	virtual pDepGraph parse(std::string) override;
	virtual ParseResult write(pDepGraph) override;

	// Inherited via DependencyParser
	virtual ParseResult getResult() override;
};

#endif // !_XMLPARSER_H_