#ifndef _CORE_CONSOLE_H_
#define _CORE_CONSOLE_H_

#include <IPCore/core_config.h>
#include <IPCore/Util/function.h>
#include <unordered_map>
#include <iostream>
#include <IPCore/Util/spinlock.h>

#define MAX_COMMAND_LENGTH 127


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
		char name[MAX_COMMAND_LENGTH + 1];
	
	};

	class CORE_API Console
	{
	public:
		Console(std::ostream&);
		bool addCommand(const char* name, const CommandFunc& cfunc);
		bool removeCommand(const char* name);
		bool call(const char* name, int argc, char** argv);
		bool in(const char* line);
		void print(const char* text);
	private:
		std::unordered_map<std::string, ConsoleCommand> m_commands;
		std::ostream& outstream;
		YieldingSpinLock<4000> m_mtx;
	};
}

#endif // !_CORE_CONSOLE_H_

