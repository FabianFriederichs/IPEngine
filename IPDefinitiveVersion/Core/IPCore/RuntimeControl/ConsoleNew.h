#ifndef _CORE_CONSOLE_H_
#define _CORE_CONSOLE_H_

#include <IPCore/core_config.h>
#include <IPCore/Util/function.h>
#include <iostream>
#include <IPCore/Core/ICoreTypes.h>

#define MAX_COMMAND_NAME_LENGTH 50
#define MAX_COMMAND_PARAMS 20
#define MAX_PARAM_LENGTH 250
#define MAX_DESC_LENGTH 5000

namespace ipengine
{
	
	class Console;
	class CORE_API ConsoleParams
	{
	private:
		class ConsoleParamImpl;	ConsoleParamImpl* cpimpl;
	public:
		ConsoleParams(ConsoleParamImpl* impl);
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
		ipchar name[MAX_COMMAND_NAME_LENGTH + 1];
		ipchar description[MAX_DESC_LENGTH + 1];
	};

	class CORE_API Console
	{
	public:
		Console(std::ostream&);
		~Console();
		bool addCommand(ipcrstr name, const CommandFunc& cfunc);
		bool addCommand(ipcrstr name, const CommandFunc& cfunc, ipcrstr description);
		bool removeCommand(ipcrstr);
		bool call(ipcrstr name, ConsoleParams& params);
		bool in(ipcrstr line);
		void print(ipcrstr text);
		void println(ipcrstr text);
		void prompt();
		void listCommands();
	private:
		class ConsoleImpl; ConsoleImpl* m_impl;
	};
}

#endif // !_CORE_CONSOLE_H_

