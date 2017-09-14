#include <RuntimeControl/Console.h>

ipengine::ConsoleCommand::ConsoleCommand()
{
}

ipengine::ConsoleCommand::ConsoleCommand(const char * name, const CommandFunc & func) :
	comfunc(func)
{
	strcpy_s(this->name, MAX_COMMAND_LENGTH, name);
}

ipengine::ConsoleCommand::~ConsoleCommand()
{

}

void ipengine::ConsoleCommand::call(int argc, char ** argv)
{
	comfunc(std::move(argc), std::move(argv));
}

ipengine::Console::Console(std::ostream & ostr) :
	outstream(ostr)
{
}

void ipengine::Console::addCommand(const char * name, const CommandFunc & cfunc)
{
	m_commands[std::string(name)] = ConsoleCommand(name, cfunc);
}

void ipengine::Console::removeCommand(const char * name)
{
	m_commands.erase(std::string(name));
}

bool ipengine::Console::call(const char * name, int argc, char ** argv)
{
	if (m_commands.find(std::string(name)) != m_commands.end())
	{
		m_commands[std::string(name)].call(argc, argv);
		return true;
	}
	return false;
}

void ipengine::Console::print(const char * text)
{
	outstream << text;
}
