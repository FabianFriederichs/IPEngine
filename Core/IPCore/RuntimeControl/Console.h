/** \addtogroup runtime
Implements runtime control and management functionality.
*  @{
*/

/*!
\file Console.h
\brief Core console functionality
*/

#ifndef _CORE_CONSOLE_H_
#define _CORE_CONSOLE_H_

#include <IPCore/core_config.h>
#include <IPCore/Util/function.h>
#include <iostream>
#include <IPCore/Core/ICoreTypes.h>

#define MAX_COMMAND_NAME_LENGTH 50
#define MAX_COMMAND_PARAMS 32
#define MAX_PARAM_LENGTH 250
#define MAX_DESC_LENGTH 5000
#define MAX_COMMAND_BUFFER_ITEMS 16

namespace ipengine
{
	
	class Console;
	/*!
	\brief Represents a parsed, ready-to-use set of parameters generated from some textual command line input.
	*/
	class CORE_API ConsoleParams
	{
	private:
		friend class CommandBufferItem;
		friend class Console;
		//! Pimpl idiom is used to hide implementation detail
		class ConsoleParamImpl;	ConsoleParamImpl* cpimpl;
	public:
		ConsoleParams(ConsoleParamImpl* impl);
		/*!
		\brief Returns the string representation of the parameter with index index.

		\param[in]	Index of the desired parameter.
		\returns	Returns the string representation of the parameter.
		*/
		ipcrstr get(ipsize index) const;
		/*!
		\brief Returns the integer representation of the parameter with index index.

		\param[in]	Index of the desired parameter.
		\returns	Returns the integer representation of the parameter if it is parsable as integer, otherwise returns 0.
		*/
		ipint64 getInt(ipsize index) const;
		/*!
		\brief Returns the floating-point representation of the parameter with index index.

		\param[in]	Index of the desired parameter.
		\returns	Returns the floating-point representation of the parameter if it is parsable as floating-point value, otherwise returns 0.0f.
		*/
		ipdouble getFloat(ipsize index) const;
		/*!
		\brief Returns the boolean representation of the parameter with index index.

		\param[in]	Index of the desired parameter.
		\returns	Returns the boolean representation of the parameter if it is parsable as boolean, otherwise returns false.
		*/
		ipbool getBool(ipsize index) const;
		/*!
		\brief Returns the number of parameters in this parameter set.

		\returns Returns the number of parameters.
		*/
		ipsize getParamCount() const;
	};

	/*!
	\brief Typedef for command handlers.

	If a module wants to add a command to the console, a callback of this type must be passed.
	*/
	using CommandFunc = function<void(const ConsoleParams&)>;

	/*!
	\brief Represents the core console.

	A Console object is initialized with an std::ostream reference for output.
	Commands can be added via the addCommand functions and removed via the removeCommand function.
	Console parses command line input and either enqueues valid commands for later execution or
	executes them immediately.

	A command in the command line has the following structure:

	\code{.unparsed}
	<commandname> ("<string value>" | <int value> | <float value> | <true|false>)*
	\endcode

	If the command "commandname" exists, it is called with the parameter set (seperated by spaces) or
	the parameter set is stored together with the command name for later execution.
	*/
	class CORE_API Console
	{
	public:
		/*!
		\brief Initializes the Console object. All output goes to the given std::ostream.
		*/
		Console(std::ostream&);
		~Console();
		/*!
		\brief Adds a new command.

		\param[in] name		Name of the command.
		\param[in] cfunc	Callback that should be called when the command is executed.
		\returns			Returns true when the command was successfully added. false is returned if the command already exists.
		*/
		bool addCommand(ipcrstr name, const CommandFunc& cfunc);
		/*!
		\brief Adds a new command.

		\param[in] name				Name of the command.
		\param[in] cfunc			Callback that should be called when the command is executed.
		\param[in] description		A string that provides further information about the command's usage. Will be displayed additionally when using the functions listCommandsDetailed and showCommandDetail
		\returns					Returns true when the command was successfully added. false is returned if the command already exists.
		*/
		bool addCommand(ipcrstr name, const CommandFunc& cfunc, ipcrstr description);
		/*!
		\brief Removes a command.

		Removes the command with the given if it exists.
		\param[in] name		Name of the command that should be removed.
		\returns			Returns true if the command was removed successfully, false if it did not exist.
		*/
		bool removeCommand(ipcrstr name);
		/*!
		\brief Calls a command with a set of parameters.

		\param[in] name		Name of the command to call.
		\param[in] params	A set of console parameters.
		\returns			Returns true if the command was found and executed successfully.
		*/
		bool call(ipcrstr name, ConsoleParams& params);
		/*!
		\brief Tries to execute input from a command line.

		The passed text is parsed. When parsing was successful and the command specified exists, the command is executed, i.e the respective callback is called with the parameters extracted from the passed piece of text.
		\param[in] line		Text that should be interpreted as command call.
		\returns			Returns true if parsing and execution of the command was successul.
		*/
		bool executeImmediate(ipcrstr line);
		/*!
		\brief Parses the given input and stores it for later execution on success.

		The passed text is parsed. When parsing was successful and the command specified exists, a reified command call is stored for later exection by the function executePendingCommands.
		\param[in] line		Text that should be interpreted as command call.
		\returns			Returns true if parsing was successful and the command specified in the command line exists.
		*/
		bool submitCommand(ipcrstr line);
		/*!
		\brief Executes all command calls that were queued up through calls to submitCommand.
		\returns Returns true if there were commands to be executed.
		*/
		bool executePendingCommands();
		/*!
		\brief Prints some text to the specified output stream.
		*/
		void print(ipcrstr text);
		/*!
		\brief Prints some text with a trailing newline character to the specified output stream.
		*/
		void println(ipcrstr text);
		/*!
		\brief Prints a prompt to the specified output stream.
		*/
		void prompt();
		/*!
		\brief Prints a list of all registered commands.
		*/
		void listCommands();
		/*!
		\brief Prints a dettailed list of all registered commands.
		*/
		void listCommandsDetailed();
		/*!
		\brief Prints some detail about the command with the passed name.
		*/
		void showCommandDetail(ipcrstr name);
	private:
		friend class ConsoleParams::ConsoleParamImpl;
		class ConsoleImpl;
		//! Hide some implementaiton detail using the pimpl idiom.
		ConsoleImpl* m_impl;
	};
}

#endif

/** @}*/