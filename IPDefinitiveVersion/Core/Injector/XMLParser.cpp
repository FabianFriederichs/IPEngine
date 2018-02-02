#include "XMLParser.h"



XMLParser::XMLParser()
{
}


XMLParser::~XMLParser()
{
}

pDepGraph XMLParser::parse(std::string)
{
	return pDepGraph();
}

ParseResult XMLParser::write(pDepGraph)
{
	return ParseResult();
}

ParseResult XMLParser::getResult()
{
	return ParseResult();
}
