#ifndef _CORE_CONSOLE_H_
#define _CORE_CONSOLE_H_

#include <core_config.h>
#include <Util/function.h>
#include <unordered_map>
#include <iostream>

#define MAX_COMMAND_LENGTH 256


namespace ipengine
{
	using CommandFunc = function<void(int, char**)>;

	class CORE_API ConsoleCommand
	{
	public:
		ConsoleCommand();
		ConsoleCommand(const char* name, const CommandFunc& func);
		ConsoleCommand(const ConsoleCommand& other) = default;
		ConsoleCommand(ConsoleCommand&& other) = default;
		ConsoleCommand& operator=(const ConsoleCommand& other) = default;
		ConsoleCommand& operator=(ConsoleCommand&& other) = default;
		~ConsoleCommand();
		void call(int argc, char** argv);
	private:
		CommandFunc comfunc;
		char name[MAX_COMMAND_LENGTH];
	
	};

	class CORE_API Console
	{
	public:
		Console(std::ostream&);
		void addCommand(const char* name, const CommandFunc& cfunc);
		void removeCommand(const char* name);
		bool call(const char* name, int argc, char** argv);
		void print(const char* text);
	private:
		std::unordered_map<std::string, ConsoleCommand> m_commands;
		std::ostream& outstream;
	};
}

#endif // !_CORE_CONSOLE_H_

