#include <IPCore/RuntimeControl/Console.h>
#include <unordered_map>
#include <cctype>
#include <mutex>
#include <type_traits>
#include <IPCore/Util/spinlock.h>
#include <atomic>

//console command implementation

ipengine::ConsoleCommand::ConsoleCommand()
{
	description[0] = 0;
}

ipengine::ConsoleCommand::ConsoleCommand(ipcrstr name, const CommandFunc & func) :
	comfunc(func)
{
	strcpy_s(this->name, MAX_COMMAND_NAME_LENGTH + 1, name);
	description[0] = 0;
}

ipengine::ConsoleCommand::ConsoleCommand(ipcrstr name, const CommandFunc & func, ipcrstr _description) :
	comfunc(func)
{
	strcpy_s(this->name, MAX_COMMAND_NAME_LENGTH + 1, name);
	strcpy_s(description, MAX_DESC_LENGTH + 1, _description);
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
	if (description[0] == 0)
		return "no description";
	else
		return description;
}

ipengine::ipcrstr ipengine::ConsoleCommand::getName()
{
	return name;
}

namespace ipengine 
{
	enum class ParamEntryType : ipengine::ipint8
	{
		Bool,
		Int,
		Float,
		String,
		Empty
	};

	struct ParamEntry
	{
		ParamEntryType type;
		ipchar value[MAX_PARAM_LENGTH + 1];

		union
		{
			ipbool boolvalue;
			ipint64 intvalue;
			ipdouble floatvalue;
		};

		ParamEntry() :
			type(ParamEntryType::Empty),
			boolvalue(false)
		{
			value[0] = 0;
		}

		ParamEntry(ipcrstr _valstr, bool _boolval) :
			type(ParamEntryType::Bool),
			boolvalue(_boolval)
		{
			strcpy_s(value, MAX_PARAM_LENGTH + 1, _valstr);
		}

		ParamEntry(ipcrstr _valstr, ipint64 _intval) :
			type(ParamEntryType::Int),
			intvalue(_intval)
		{
			strcpy_s(value, MAX_PARAM_LENGTH + 1, _valstr);
		}

		ParamEntry(ipcrstr _valstr, ipdouble _floatval) :
			type(ParamEntryType::Float),
			floatvalue(_floatval)
		{
			strcpy_s(value, MAX_PARAM_LENGTH + 1, _valstr);
		}

		ParamEntry(ipcrstr _valstr) :
			type(ParamEntryType::String),
			boolvalue(false)
		{
			strcpy_s(value, MAX_PARAM_LENGTH + 1, _valstr);
		}

		ParamEntry(const ParamEntry& other) :
			type(other.type)
		{
			strcpy_s(value, MAX_PARAM_LENGTH + 1, other.value);

			switch (other.type)
			{
			case ParamEntryType::Bool:
				boolvalue = other.boolvalue;
				break;
			case ParamEntryType::Int:
				intvalue = other.intvalue;
				break;
			case ParamEntryType::Float:
				floatvalue = other.floatvalue;
				break;
			default:
				break;
			}
		}

		ParamEntry(ParamEntry&& other) :
			type(other.type)
		{
			strcpy_s(value, MAX_PARAM_LENGTH + 1, other.value);
			other.value[0] = 0;
			switch (other.type)
			{
			case ParamEntryType::Bool:
				boolvalue = other.boolvalue;
				break;
			case ParamEntryType::Int:
				intvalue = other.intvalue;
				break;
			case ParamEntryType::Float:
				floatvalue = other.floatvalue;
				break;
			default:
				break;
			}
			other.type = ParamEntryType::Empty;
		}

		ParamEntry& operator=(const ParamEntry& other)
		{
			if (this == &other)
				return *this;

			strcpy_s(value, MAX_PARAM_LENGTH + 1, other.value);
			type = other.type;

			switch (type)
			{
			case ParamEntryType::Bool:
				boolvalue = other.boolvalue;
				break;
			case ParamEntryType::Int:
				intvalue = other.intvalue;
				break;
			case ParamEntryType::Float:
				floatvalue = other.floatvalue;
				break;
			default:
				break;
			}

			return *this;
		}

		ParamEntry& operator=(ParamEntry&& other)
		{
			if (this == &other)
				return *this;

			strcpy_s(value, MAX_PARAM_LENGTH + 1, other.value);
			other.value[0] = 0;
			type = other.type;
			other.type = ParamEntryType::Empty;

			switch (type)
			{
			case ParamEntryType::Bool:
				boolvalue = other.boolvalue;
				break;
			case ParamEntryType::Int:
				intvalue = other.intvalue;
				break;
			case ParamEntryType::Float:
				floatvalue = other.floatvalue;
				break;
			default:
				break;
			}

			return *this;
		}

		~ParamEntry()
		{

		}
	};

	class ConsoleParams::ConsoleParamImpl
	{
	public:
		ConsoleParamImpl(const ConsoleParamImpl& other) :
			paramcount(other.paramcount)
		{
			for (ipsize i = 0; i < other.paramcount; ++i)
			{
				entries[i] = other.entries[i];
			}
		}

		ConsoleParamImpl() :
			paramcount(0)
		{}

		ConsoleParamImpl& operator=(const ConsoleParamImpl& other)
		{
			if (this == &other)
				return *this;

			paramcount = other.paramcount;
			for (ipsize i = 0; i < other.paramcount; ++i)
			{
				entries[i] = other.entries[i];
			}

			return *this;
		}

		ipcrstr get(ipsize index)
		{
			if (index < MAX_COMMAND_PARAMS)
			{
				return entries[index].value;
			}
			return "";
		}		

		ipbool getBool(ipsize index)
		{
			if (index < MAX_COMMAND_PARAMS && entries[index].type == ParamEntryType::Bool)
			{
				return entries[index].boolvalue;
			}
			return false;
		}

		ipint64 getInt(ipsize index)
		{
			if (index < MAX_COMMAND_PARAMS && entries[index].type == ParamEntryType::Int)
			{
				return entries[index].intvalue;
			}
			return 0;
		}

		ipdouble getFloat(ipsize index)
		{
			if (index < MAX_COMMAND_PARAMS && entries[index].type == ParamEntryType::Float)
			{
				return entries[index].floatvalue;
			}
			return 0.0;
		}

		ipsize getParamCount()
		{
			return paramcount;
		}

	private:
		friend class Console::ConsoleImpl;

		void addEntry(const ParamEntry& e)
		{
			if (paramcount == MAX_COMMAND_PARAMS)
				throw std::logic_error("Too many parameters.");

			entries[paramcount++] = e;
		}

		ParamEntry entries[MAX_COMMAND_PARAMS];
		ipsize paramcount;
	};

	struct CommandBufferItem
	{		
		ConsoleParams::ConsoleParamImpl params;
		ipcrstr commandname;
	};
}

ipengine::ConsoleParams::ConsoleParams(ConsoleParamImpl * impl) :
	cpimpl(impl)
{
}

ipengine::ipcrstr ipengine::ConsoleParams::get(ipsize index) const
{
	return cpimpl->get(index);
}

ipengine::ipint64 ipengine::ConsoleParams::getInt(ipsize index) const
{
	return cpimpl->getInt(index);
}

ipengine::ipdouble ipengine::ConsoleParams::getFloat(ipsize index) const
{
	return cpimpl->getFloat(index);
}

ipengine::ipbool ipengine::ConsoleParams::getBool(ipsize index) const
{
	return cpimpl->getBool(index);
}

ipengine::ipsize ipengine::ConsoleParams::getParamCount() const
{
	return cpimpl->getParamCount();
}

class ipengine::Console::ConsoleImpl
{
private:
	enum class TokType
	{
		String,
		Name,
		Float,
		Int,
		Bool,
		Invalid,
		EndOfFile
	};

	struct Token
	{
		Token() :
			type(TokType::Invalid),
			value(""),
			intvalue(0)
		{
		}

		Token(TokType _type, const ipstring& sval) :
			type(_type),
			value(sval),
			intvalue(0)
		{}

		Token(TokType _type, const ipstring& sval, ipint64 intval) :
			type(_type),
			value(sval),
			intvalue(intval)
		{
		}

		Token(TokType _type, const ipstring& sval, ipdouble floatval) :
			type(_type),
			value(sval),
			floatvalue(floatval)
		{
		}

		Token(TokType _type, const ipstring& sval, ipbool boolval) :
			type(_type),
			value(sval),
			boolvalue(boolval)
		{}

		TokType type;
		ipstring value;
		union
		{
			ipbool boolvalue;
			ipint64 intvalue;
			ipdouble floatvalue;
		};
	};

	struct Scanner
	{
		ipstring file;
		size_t pos;
		size_t filesize;
		size_t row;
		size_t col;
		Token lh;

		Scanner(ipengine::ipcrstr _line)
		{
			file = std::string(_line);
			filesize = file.length();
			pos = 0;
			row = 0;
			col = 0;
			lh = Token{ TokType::Invalid, "" };
			next();
		}

		ipstring getErrorString(const ipstring& msg)
		{
			ipstring estr = "Console parsing error at position: ";
			estr += std::to_string(col + 1) + "\n";
			estr += msg;
			return estr;
		}

		bool eof()
		{
			return pos >= filesize;
		}

		const Token& next()
		{
			if (lh.type == TokType::EndOfFile)
				return lh;

			if (eof())
			{
				lh = Token(TokType::EndOfFile, "<eof>");
				return lh;
			}

			//handle spaces and tabs

			if (std::isspace(static_cast<unsigned char>(file[pos])))
			{
				++pos;
				while (!eof() && std::isspace(static_cast<unsigned char>(file[pos])))
				{
					++pos;
					++col;
				}
			}

			if (file[pos] == '.' || file[pos] == '-' || std::isdigit(static_cast<unsigned char>(file[pos])))
			{
				//test for float or integer, maybe include e notation
				ipstring num("");
				ipstring epart("");
				int pot = 0;
				size_t curpos = pos;
				bool dot = false;
				bool sign = false;
				if (file[pos] == '-')
				{
					sign = true;
					++curpos;
					++col;
				}
				while (!eof() && (file[curpos] == '.' || std::isdigit(static_cast<unsigned char>(file[curpos]))))
				{
					if (std::isdigit(static_cast<unsigned char>(file[curpos])))
					{
						num += file[curpos];
						if (dot)
							pot--;
						curpos++;
						col++;
						continue;
					}

					if (file[curpos] == '.' && !dot)
					{
						dot = true;
						num += '.';
						curpos++;
						col++;
						continue;
					}

					if (file[curpos] == '.' && dot)
					{
						throw std::logic_error(getErrorString("Wrong floating point format"));
					}
				}

				pos = curpos;

				if (!eof() && file[pos] == 'e')
				{
					++pos;
					++col;
					epart += 'e';
					bool esign = false;
					if (!eof() && file[pos] == '-')
					{
						esign = true;
						epart += '-';
						++pos;
						++col;
					}
					int epot = 0;
					while (!eof() && std::isdigit(static_cast<unsigned char>(file[pos])))
					{
						epart += file[pos];
						switch (file[pos])
						{
						case '0':
							epot = epot * 10 + 0;
							break;
						case '1':
							epot = epot * 10 + 1;
							break;
						case '2':
							epot = epot * 10 + 2;
							break;
						case '3':
							epot = epot * 10 + 3;
							break;
						case '4':
							epot = epot * 10 + 4;
							break;
						case '5':
							epot = epot * 10 + 5;
							break;
						case '6':
							epot = epot * 10 + 6;
							break;
						case '7':
							epot = epot * 10 + 7;
							break;
						case '8':
							epot = epot * 10 + 8;
							break;
						case '9':
							epot = epot * 10 + 9;
						}
						++pos;
						++col;
					}
					epot = (esign ? -epot : epot);
					pot += epot;
				}

				ipint64 rawnum = 0;
				for (size_t i = 0; i < num.length(); i++)
				{
					switch (num[i])
					{
					case '0':
						rawnum = rawnum * 10 + 0;
						break;
					case '1':
						rawnum = rawnum * 10 + 1;
						break;
					case '2':
						rawnum = rawnum * 10 + 2;
						break;
					case '3':
						rawnum = rawnum * 10 + 3;
						break;
					case '4':
						rawnum = rawnum * 10 + 4;
						break;
					case '5':
						rawnum = rawnum * 10 + 5;
						break;
					case '6':
						rawnum = rawnum * 10 + 6;
						break;
					case '7':
						rawnum = rawnum * 10 + 7;
						break;
					case '8':
						rawnum = rawnum * 10 + 8;
						break;
					case '9':
						rawnum = rawnum * 10 + 9;
					case '.':
						break;
					}
				}

				if (dot && num.length() >= 2) //float
				{
					double res = (sign ? -1.0 : 1.0) * static_cast<double>(rawnum) * std::pow(10.0, static_cast<double>(pot));
					lh = Token(TokType::Float, num + epart, res);
					return lh;
				}
				else if (!dot && num.length() >= 1)
				{
					ipint64 res = (sign ? -1 : 1) * rawnum * static_cast<ipint64>(std::pow(ipint64(10), static_cast<ipint64>(pot)));
					lh = Token(TokType::Int, num + epart, res);
					return lh;
				}
				else
				{
					throw std::logic_error(getErrorString("Wrong number format"));
				}
			}

			if (file[pos] == '"')
			{
				ipstring str = "";
				ipstring::value_type c;
				++pos;
				++col;
				bool stringclosed = false;
				while (!eof())
				{
					c = file[pos++];
					++col;
					if (c != '"')
						str += c;
					else
					{
						stringclosed = true;
						break;
					}
				}
				if (!stringclosed)
				{
					throw std::logic_error(getErrorString("Unclosed string literal"));
				}
				lh = Token(TokType::String, str);
				return lh;
			}

			//name or true/false
			if (std::isalpha(static_cast<unsigned char>(file[pos])) || file[pos] == '_')
			{
				ipstring idname = "";
				idname += file[pos++];
				++col;
				while (!eof() && (std::isalnum(static_cast<unsigned char>(file[pos])) || file[pos] == '_' || file[pos] == '.'))
				{
					idname += file[pos++];
					++col;
				}
				//check for keywords
				if (idname == "true")
				{
					lh = Token(TokType::Bool, "true", 1ll);
				}
				else if (idname == "false")
				{
					lh = Token(TokType::Bool, "false", 0ll);
				}
				else
				{
					lh = Token(TokType::Name, idname);
				}
				return lh;
			}

			ipstring estr = "Unexpected '" + lh.value + "'";
			throw std::logic_error(getErrorString(estr));
		}

		Scanner& match(TokType mt)
		{
			if (lh.type == mt)
			{
				next();
				return *this;
			}
			ipstring estr = "Unexpected '" + lh.value + "'";
			throw std::logic_error(getErrorString(estr));
		}

		void throwUnexpected()
		{
			ipstring estr = "Unexpected '" + lh.value + "'";
			throw std::logic_error(getErrorString(estr));
		}

		const Token& lookahead()
		{
			return lh;
		}
	};

public:
	ConsoleImpl(std::ostream& stream) :
		outstream(stream),
		m_pendingcmds(0)
	{

	}

	~ConsoleImpl()
	{

	}

	bool addCommand(ipcrstr name, const CommandFunc & cfunc)
	{
		autolock lock(m_mtx);
		if (std::strlen(name) <= MAX_COMMAND_NAME_LENGTH)
		{
			m_commands[std::string(name)] = ConsoleCommand(name, cfunc);
			return true;
		}
		return false;
	}
	bool addCommand(ipcrstr name, const CommandFunc & cfunc, ipcrstr description)
	{
		autolock lock(m_mtx);
		if (std::strlen(name) <= MAX_COMMAND_NAME_LENGTH)
		{
			m_commands[std::string(name)] = ConsoleCommand(name, cfunc, description);
			return true;
		}
		return false;
	}
	bool removeCommand(ipcrstr name)
	{
		autolock lock(m_mtx);
		return m_commands.erase(std::string(name)) > 0;
	}
	bool call(ipcrstr name, ConsoleParams & params)
	{
		autolock lock(m_mtx);
		if (m_commands.find(std::string(name)) != m_commands.end())
		{
			m_commands[std::string(name)].call(params);
			return true;
		}
		return false;
	}
	void print(ipcrstr text)
	{
		std::lock_guard<YieldingSpinLock<4000>> lock(m_outmtx);
		outstream << text;
	}
	void println(ipcrstr text)
	{
		std::lock_guard<YieldingSpinLock<4000>> lock(m_outmtx);
		outstream << text << "\n";
	}
	void prompt()
	{
		print(">");
	}
	void listCommands()
	{
		autolock lock(m_mtx);
		println("List of available console commands:\n--------");
		for (auto& c : m_commands)
		{
			println("Name:");
			println(c.second.getName());
			println("\nDescription:");
			println(c.second.getDescription());
			println("--------");
		}
	}
	bool executeImmediate(ipcrstr line)
	{
		CommandBufferItem item;
		if (parseLine(line, item))
		{
			return call(item.commandname, ConsoleParams(&item.params));
			prompt();
		}
		return false;
	}
	bool submitCommand(ipcrstr line)
	{		
		//TODO: check if this test is enough
		if (m_pendingcmds.load(std::memory_order_relaxed) == MAX_COMMAND_BUFFER_ITEMS)
		{
			println("Command buffer is full. Please try again.");
			prompt();
			return false;
		}
		CommandBufferItem item;
		if (parseLine(line, item))
		{
			autolock lock(m_mtx);
			m_cmdbuffer[m_pendingcmds.fetch_add(1, std::memory_order_relaxed)] = item;
			return true;
		}
		prompt();
		return false;
	}
	bool executePendingCommands()
	{
		if (auto n = m_pendingcmds.load(std::memory_order_relaxed))
		{
			autolock lock(m_mtx);
			for (ipsize i = 0; i < n; ++i)
			{
				call(m_cmdbuffer[i].commandname, ConsoleParams(&m_cmdbuffer[i].params));
				prompt();
			}
			m_pendingcmds.store(0, std::memory_order_relaxed);
			return true;
		}
		return false;
	}
	bool parseLine(ipcrstr line, CommandBufferItem& item)
	{
		try
		{
			Scanner scan(line);
			if (scan.lookahead().type == TokType::Name)
			{
				auto& nit = m_commands.find(std::string(scan.lookahead().value));
				if (nit != m_commands.end())
				{
					item.commandname = nit->second.getName();
					scan.match(TokType::Name);
					return parseArguments(scan, item);
				}
				else
				{
					throw std::logic_error("Unknown command '" + std::string(scan.lookahead().value) + "'");
				}
			}
			else
			{
				scan.throwUnexpected();
			}
		}
		catch (std::exception& ex)
		{
			std::string msg("Console parsing error: ");
			msg += ex.what();
			println(msg.c_str());
			return false;
		}
	}
	bool parseArguments(Scanner& scan, CommandBufferItem& item)
	{
		while (scan.lookahead().type != TokType::EndOfFile)
		{
			if (scan.lookahead().type == TokType::String)
			{
				item.params.addEntry(ParamEntry(scan.lookahead().value.c_str()));
				scan.match(TokType::String);
			}
			else if (scan.lookahead().type == TokType::Int)
			{
				item.params.addEntry(ParamEntry(scan.lookahead().value.c_str(), scan.lookahead().intvalue));
				scan.match(TokType::Int);
			}
			else if (scan.lookahead().type == TokType::Float)
			{
				item.params.addEntry(ParamEntry(scan.lookahead().value.c_str(), scan.lookahead().floatvalue));
				scan.match(TokType::Float);
			}
			else if (scan.lookahead().type == TokType::Bool)
			{
				item.params.addEntry(ParamEntry(scan.lookahead().value.c_str(), scan.lookahead().boolvalue));
				scan.match(TokType::Bool);
			}
			else if (scan.lookahead().type == TokType::EndOfFile)
			{
				break;
			}
			else
			{
				scan.throwUnexpected();
			}
		}
		scan.match(TokType::EndOfFile);
		return true;
	}

private:
	using autolock = std::lock_guard<std::recursive_mutex>;
	std::unordered_map<std::string, ConsoleCommand> m_commands;
	std::ostream& outstream;
	std::recursive_mutex m_mtx;
	YieldingSpinLock<4000> m_outmtx;

	CommandBufferItem m_cmdbuffer[MAX_COMMAND_BUFFER_ITEMS];
	std::atomic<ipsize> m_pendingcmds;
};

ipengine::Console::Console(std::ostream & stream) :
	m_impl(new ConsoleImpl(stream))
{
}
ipengine::Console::~Console()
{
	if (m_impl)
		delete m_impl;
}
bool ipengine::Console::addCommand(ipcrstr name, const CommandFunc & cfunc)
{
	return m_impl->addCommand(name, cfunc);
}
bool ipengine::Console::addCommand(ipcrstr name, const CommandFunc & cfunc, ipcrstr description)
{
	return m_impl->addCommand(name, cfunc, description);
}
bool ipengine::Console::removeCommand(ipcrstr name)
{
	return m_impl->removeCommand(name);
}
bool ipengine::Console::call(ipcrstr name, ConsoleParams & params)
{
	return m_impl->call(name, params);
}
bool ipengine::Console::executeImmediate(ipcrstr line)
{
	return m_impl->executeImmediate(line);
}
bool ipengine::Console::submitCommand(ipcrstr line)
{
	return m_impl->submitCommand(line);
}
bool ipengine::Console::executePendingCommands()
{
	return m_impl->executePendingCommands();
}
void ipengine::Console::print(ipcrstr text)
{
	m_impl->print(text);
}
void ipengine::Console::println(ipcrstr text)
{
	m_impl->println(text);
}
void ipengine::Console::prompt()
{
	m_impl->prompt();
}
void ipengine::Console::listCommands()
{
	m_impl->listCommands();
}