#ifndef _CORE_CONSOLE_H_
#define _CORE_CONSOLE_H_

#include <IPCore/core_config.h>
#include <IPCore/Util/function.h>
#include <unordered_map>
#include <iostream>
#include <IPCore/Util/spinlock.h>
#include <cctype>
#include <IPCore/Core/ICoreTypes.h>
#include <mutex>

#define MAX_COMMAND_LENGTH 127
#define MAX_COMMAND_PARAMS 32
#define MAX_DESC_LENGTH 4096


namespace ipengine
{
	class Console;
	class ConsoleParams
	{
	private:
		iprstr* parambuffer;
		const ipsize paramcount;
		//ConsoleParams(const ipchar(*_parambuffer)[MAX_COMMAND_LENGTH + 1], ipsize _paramcount);
	public:
		ConsoleParams(iprstr _params[], ipsize _paramcount);
		ipcrstr get(ipsize index) const;
		ipint64 getInt(ipsize index) const;
		ipdouble getFloat(ipsize index) const;
		ipbool getBool(ipsize index) const;
		ipsize getParamCount() const;
	};

	using CommandFunc = function<void(const ConsoleParams&)>;

	class CORE_API ConsoleCommand
	{
	public:
		ConsoleCommand();
		ConsoleCommand(ipcrstr name, const CommandFunc& func);
		ConsoleCommand(ipcrstr name, const CommandFunc& func, ipcrstr description);
		ConsoleCommand(const ConsoleCommand& other) = default;
		ConsoleCommand(ConsoleCommand&& other) = default;
		ConsoleCommand& operator=(const ConsoleCommand& other) = default;
		ConsoleCommand& operator=(ConsoleCommand&& other) = default;
		~ConsoleCommand();
		void call(const ConsoleParams& params);
		ipcrstr getDescription();
		ipcrstr getName();
	private:
		CommandFunc comfunc;
		ipchar name[MAX_COMMAND_LENGTH + 1];
		std::string description;
	};

	class CORE_API Console
	{
	public:
		Console(std::ostream&);
		~Console();
		bool addCommand(ipcrstr name, const CommandFunc& cfunc);
		bool addCommand(ipcrstr name, const CommandFunc& cfunc, ipcrstr description);
		bool removeCommand(ipcrstr);
		bool call(ipcrstr name, ConsoleParams params);
		bool in(ipcrstr line);
		void print(ipcrstr text);
		void println(ipcrstr text);
		void listCommands();
	private:
		std::unordered_map<std::string, ConsoleCommand> m_commands;
		std::ostream& outstream;
		//direct call buffer
		iprstr* argbuffer;
		
		//TODO: implement recursive spinock
		std::recursive_mutex m_mtx;
		using autolock = std::lock_guard<std::recursive_mutex>;

		//Spinlock for prints
		YieldingSpinLock<4000> m_outmtx;
	};
}

#endif // !_CORE_CONSOLE_H_

