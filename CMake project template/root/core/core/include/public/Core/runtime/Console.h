#ifndef _CORE_CONSOLE_H_
#define _CORE_CONSOLE_H_

#include <core/core_config.h>
#include <core/util/function.h>
#include <iostream>
#include <core/core_types.h>

#define MAX_COMMAND_NAME_LENGTH 50
#define MAX_COMMAND_PARAMS 32
#define MAX_PARAM_LENGTH 250
#define MAX_DESC_LENGTH 5000
#define MAX_COMMAND_BUFFER_ITEMS 16

namespace ipengine
{
	
	class Console;
	class CORE_API ConsoleParams
	{
	private:
		friend class CommandBufferItem;
		friend class Console;
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
		bool executeImmediate(ipcrstr line);
		bool submitCommand(ipcrstr line);
		bool executePendingCommands();
		void print(ipcrstr text);
		void println(ipcrstr text);
		void prompt();
		void listCommands();
		void listCommandsDetailed();
		void showCommandDetail(ipcrstr name);
	private:
		friend class ConsoleParams::ConsoleParamImpl;
		class ConsoleImpl; ConsoleImpl* m_impl;
	};
}

#endif // !_CORE_CONSOLE_H_