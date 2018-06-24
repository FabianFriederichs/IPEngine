#ifndef _DEPENDENCYPARSER_H_
#define _DEPENDENCYPARSER_H_
#include "DependencyGraph.h"

#pragma once
class DependencyParser
{
public:
	class ParserError : public std::exception
	{
		const char* file;
		int line;
		const char* func;
		const char* info;

	public:
		ParserError(const char* msg, const char* file_, int line_, const char* func_, const char* info_ = "") : std::exception(msg),
			file(file_),
			line(line_),
			func(func_),
			info(info_)
		{
		}

		const char* get_file() const { return file; }
		int get_line() const { return line; }
		const char* get_func() const { return func; }
		const char* get_info() const { return info; }
	};


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