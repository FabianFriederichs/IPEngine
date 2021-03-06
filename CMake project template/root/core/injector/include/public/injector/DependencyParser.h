#ifndef _DEPENDENCYPARSER_H_
#define _DEPENDENCYPARSER_H_
#include <injector/DependencyGraph.h>

#pragma once
class DependencyParser
{
public:
	enum ParseResult
	{
		WRITING_FAILED,
		READING_FAILED,
		WRITING_SUCCESS,
		READING_SUCCESS
	};
	using pDepGraph = std::shared_ptr<DGStuff::DependencyGraph>;
	virtual pDepGraph parse(std::string) = 0;
	virtual ParseResult write(DGStuff::DependencyGraph&, std::string) = 0;

	virtual ParseResult getResult() = 0;
};


#endif // !_DEPENDENCYPARSER_H_