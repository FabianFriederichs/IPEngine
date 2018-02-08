#include <IPCore/RuntimeControl/Console.h>
#include <mutex>
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

bool ipengine::Console::addCommand(const char * name, const CommandFunc & cfunc)
{
	std::lock_guard<YieldingSpinLock<4000>> lock(m_mtx);
	if (std::strlen(name) <= MAX_COMMAND_LENGTH)
	{
		m_commands[std::string(name)] = ConsoleCommand(name, cfunc);
		return true;
	}
	return false;
}

bool ipengine::Console::removeCommand(const char * name)
{
	std::lock_guard<YieldingSpinLock<4000>> lock(m_mtx);
	return m_commands.erase(std::string(name)) > 0;
}

bool ipengine::Console::call(const char * name, int argc, char ** argv)
{
	std::lock_guard<YieldingSpinLock<4000>> lock(m_mtx);
	if (m_commands.find(std::string(name)) != m_commands.end())
	{
		m_commands[std::string(name)].call(argc, argv);
		return true;
	}
	return false;
}

bool ipengine::Console::in(const char * line)
{
	std::lock_guard<YieldingSpinLock<4000>> lock(m_mtx);
	//TODO: do parsing of one line here and call the command
	return false;
}

void ipengine::Console::print(const char * text)
{
	std::lock_guard<YieldingSpinLock<4000>> lock(m_mtx);
	outstream << text;
}
