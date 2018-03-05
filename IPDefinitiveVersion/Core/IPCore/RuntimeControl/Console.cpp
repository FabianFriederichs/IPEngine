#include <IPCore/RuntimeControl/Console.h>
#include <mutex>
ipengine::ConsoleCommand::ConsoleCommand()
{
}

ipengine::ConsoleCommand::ConsoleCommand(ipcrstr name, const CommandFunc & func) :
	comfunc(func)
{
	strcpy_s(this->name, MAX_COMMAND_LENGTH, name);
}

ipengine::ConsoleCommand::ConsoleCommand(ipcrstr name, const CommandFunc & func, ipcrstr _description) :
	comfunc(func),
	description(_description)
{
	strcpy_s(this->name, MAX_COMMAND_LENGTH, name);
}

ipengine::ConsoleCommand::~ConsoleCommand()
{

}

void ipengine::ConsoleCommand::call(const ipengine::ConsoleParams& params)
{
	comfunc(params);
}

ipengine::ipcrstr ipengine::ConsoleCommand::getDescription()
{
	if (description.empty())
		return "no description";
	else
		return description.c_str();
}

ipengine::ipcrstr ipengine::ConsoleCommand::getName()
{
	return name;
}

ipengine::Console::Console(std::ostream & ostr) :
	outstream(ostr)
{
	argbuffer = new iprstr[MAX_COMMAND_PARAMS + 1];
	for (ipsize i = 0; i < MAX_COMMAND_PARAMS; ++i)
	{
		argbuffer[i] = new ipchar[MAX_COMMAND_LENGTH + 1];
	}
}

ipengine::Console::~Console()
{
	for (ipsize i = 0; i < MAX_COMMAND_PARAMS; ++i)
	{
		delete [] argbuffer[i];
	}
	delete [] argbuffer;
}

ipengine::ipbool ipengine::Console::addCommand(ipcrstr name, const CommandFunc & cfunc)
{
	autolock lock(m_mtx);
	if (std::strlen(name) <= MAX_COMMAND_LENGTH)
	{
		m_commands[std::string(name)] = ConsoleCommand(name, cfunc);
		return true;
	}
	return false;
}

bool ipengine::Console::addCommand(ipcrstr name, const CommandFunc & cfunc, ipcrstr description)
{
	autolock lock(m_mtx);
	if (std::strlen(name) <= MAX_COMMAND_LENGTH)
	{
		m_commands[std::string(name)] = ConsoleCommand(name, cfunc, description);
		return true;
	}
	return false;
}

ipengine::ipbool ipengine::Console::removeCommand(ipengine::ipcrstr name)
{
	autolock lock(m_mtx);
	return m_commands.erase(std::string(name)) > 0;
}

ipengine::ipbool ipengine::Console::call(ipengine::ipcrstr name, ipengine::ConsoleParams params)
{
	autolock lock(m_mtx);
	if (m_commands.find(std::string(name)) != m_commands.end())
	{
		m_commands[std::string(name)].call(params);
		return true;
	}
	return false;
}

bool ipengine::Console::in(ipcrstr line)
{
	//TODO: do parsing of one line here and call the command
	//char params[MAX_COMMAND_PARAMS + 1][MAX_COMMAND_LENGTH + 1];
	ipsize i1 = 0;
	ipsize i2 = 0;

	ipcrstr rp = line;
	ipchar cmdname[MAX_COMMAND_LENGTH + 1];
	ipchar last = 0;
	autolock lock(m_mtx);
	while (*rp)
	{
		if (*rp == ' ' && last == ' ')
		{
			++rp;
		}
		else if (*rp == ' ')
		{
			if (!last) //line begins with space => ignore
			{
				last = ' ';
				++rp;
			}
			else
			{
				if (i1)
					argbuffer[i1 - 1][i2] = 0;
				else
					cmdname[i2] = 0; //close the current segment string
				i2 = 0;
				++i1;
				++rp;
				last = ' ';
			}
		}
		else if (*rp == '"')
		{
			if (i1 == 0)
				return false;
			++rp;
			bool closed = false;
			while (*rp)
			{
				if (*rp == '"')
				{
					closed = true;
					last = '"';
					++rp;
					break;
				}
				else
				{
					argbuffer[i1 - 1][i2] = *rp;
					++rp;
					++i2;
					if (i2 >= MAX_COMMAND_LENGTH)
					{
						return false;
					}
				}
			}
			if (!closed)
			{
				return false;
			}
		}
		else
		{
			if (i1)
				argbuffer[i1 - 1][i2] = *rp;
			else
				cmdname[i2] = *rp;
			last = *rp;
			++rp;
			++i2;
			if (i2 >= MAX_COMMAND_LENGTH)
			{
				return false;
			}
		}
	}
	//close last thingy
	if (last != ' ')
	{
		if (i1)
			argbuffer[i1 - 1][i2] = 0;
		else
			cmdname[i2] = 0;
	}

	ConsoleParams p(argbuffer, i1);
	return call(cmdname, p);
}

void ipengine::Console::print(ipengine::ipcrstr text)
{
	std::lock_guard<YieldingSpinLock<4000>> lock(m_outmtx);
	outstream << text;
}

void ipengine::Console::println(ipcrstr text)
{
	std::lock_guard<YieldingSpinLock<4000>> lock(m_outmtx);
	outstream << text << "\n";
}

void ipengine::Console::listCommands()
{
	autolock lock(m_mtx);
	println("List of available console commands:\n--------");
	for (auto& c : m_commands)
	{
		println("Name:");
		println(c.second.getName());
		println("\nDescription:");
		println(c.second.getDescription());
		println("\n");
	}
}

ipengine::ConsoleParams::ConsoleParams(iprstr _parambuffer[], ipsize _paramcount) :
	paramcount(_paramcount),
	parambuffer(_parambuffer)
{
}

ipengine::ipcrstr ipengine::ConsoleParams::get(ipengine::ipsize index) const
{
	if (index < paramcount)
	{
		return parambuffer[index];
	}
	return nullptr;
}

ipengine::ipint64 ipengine::ConsoleParams::getInt(ipsize index) const
{
	if(auto s = get(index))
		return std::strtoll(s, nullptr, 0);
	return 0;
}

ipengine::ipdouble ipengine::ConsoleParams::getFloat(ipsize index) const
{
	if (auto s = get(index))
		return std::strtod(get(index), nullptr);
	return 0.0;
}

ipengine::ipbool ipengine::ConsoleParams::getBool(ipsize index) const
{
	if (auto s = get(index))
	{
		if (strncmp(get(index), "true", MAX_COMMAND_LENGTH) == 0)
			return true;
		else
			return false;
	}
	return false;
}

ipengine::ipsize ipengine::ConsoleParams::getParamCount() const
{
	return paramcount;
}
