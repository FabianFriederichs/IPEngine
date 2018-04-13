#include <IPCore/RuntimeControl/Console.h>
#include <unordered_map>
#include <cctype>
#include <mutex>
#include <type_traits>
#include <IPCore/Util/spinlock.h>

//console command implementation

#pragma region console command implementation

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

#pragma endregion

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

		ParamEntry(const ipstring& _name, ipcrstr _valstr, bool _boolval) :
			type(ParamEntryType::Bool),
			boolvalue(_boolval)
		{
			strcpy_s(value, MAX_PARAM_LENGTH + 1, _valstr);
		}

		ParamEntry(const ipstring& _name, ipcrstr _valstr, ipint64 _intval) :
			type(ParamEntryType::Int),
			intvalue(_intval)
		{
			strcpy_s(value, MAX_PARAM_LENGTH + 1, _valstr);
		}

		ParamEntry(const ipstring& _name, ipcrstr _valstr, ipdouble _floatval) :
			type(ParamEntryType::Float),
			floatvalue(_floatval)
		{
			strcpy_s(value, MAX_PARAM_LENGTH + 1, _valstr);
		}

		ParamEntry(const ipstring& _name, ipcrstr _valstr) :
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

		ConsoleParamImpl& operator=(const ConsoleParamImpl& other)
		{
			paramcount = other.paramcount;
			for (ipsize i = 0; i < other.paramcount; ++i)
			{
				entries[i] = other.entries[i];
			}
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

		ParamEntry entries[MAX_COMMAND_PARAMS];
		ipsize paramcount;
	};
}

#pragma region console params implementation
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
#pragma endregion

class ipengine::Console::ConsoleImpl
{
public:
	ConsoleImpl(std::ostream& stream) :
		outstream(stream)
	{

	}
	~ConsoleImpl()
	{

	}
private:
	using autolock = std::lock_guard<std::recursive_mutex>;
	std::unordered_map<std::string, ConsoleCommand> m_commands;
	std::ostream& outstream;
	iprstr* argbuffer;
	std::recursive_mutex m_mtx;
	YieldingSpinLock<4000> m_outmtx;
};

#pragma region console implementation
ipengine::Console::Console(std::ostream &)
{
}
ipengine::Console::~Console()
{
}
bool ipengine::Console::addCommand(ipcrstr name, const CommandFunc & cfunc)
{
	return false;
}
bool ipengine::Console::addCommand(ipcrstr name, const CommandFunc & cfunc, ipcrstr description)
{
	return false;
}
bool ipengine::Console::removeCommand(ipcrstr)
{
	return false;
}
bool ipengine::Console::call(ipcrstr name, ConsoleParams & params)
{
	return false;
}
bool ipengine::Console::in(ipcrstr line)
{
	return false;
}
void ipengine::Console::print(ipcrstr text)
{
}
void ipengine::Console::println(ipcrstr text)
{
}
void ipengine::Console::prompt()
{
}
void ipengine::Console::listCommands()
{
}
#pragma endregion