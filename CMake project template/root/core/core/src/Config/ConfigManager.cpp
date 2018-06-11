#include <core/config/ConfigManager.h>
#include <core/util/spinlock.h>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <cctype>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <mutex>

//implementation

class ipengine::ConfigManager::ConfigImpl
{
public:
	//internal types --------------------------------------------------------------------------------
	enum class TokType
	{
		Tab,
		String,
		Name,
		Float,
		Int,
		Bool,
		Minus,
		EqualSign,
		Invalid,
		LineBreak,
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

		Scanner(std::istream& _stream)
		{
			std::stringstream ss;
			ss << _stream.rdbuf();
			file = ss.str();
			filesize = file.length();
			pos = 0;
			row = 0;
			col = 0;
			lh = Token{TokType::Invalid, ""};
			next();
		}

		ipstring getErrorString(const ipstring& msg)
		{
			ipstring estr = "Config file parsing error at line: ";
			estr += std::to_string(row) + ", column: ";
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
			if (file[pos] == '\t')
			{
				lh = Token(TokType::Tab, "<tab>");
				++pos;
				col += 4;
				return lh;
			}

			if (file[pos] == ' ')
			{
				unsigned int sc = 1;
				++pos;
				while (!eof() && file[pos] == ' ' && sc < 4)
				{
					++pos;
					++sc;
				}
				if (sc == 4)
				{
					lh = Token(TokType::Tab, "<tab>");
					return lh;
				}
			}

			//comments
			if (file[pos] == '#')
			{
				while (!eof() && file[pos] != '\n') ++pos;
			}

			//linebreaks
			if (file[pos] == '\n' ||
				((pos + 1 < filesize) &&
				 file[pos] == '\r' &&
				 file[pos + 1] == '\n'))
			{
				col = 0;
				//position
				if (file[pos] == '\r' && file[pos + 1] == '\n')
					pos += 2;
				else
					pos++;
				++row;
				lh = Token(TokType::LineBreak, "\n");
				return lh;
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

			/*if (file[pos] == '-')
			{
				lh = Token(TokType::Minus, "-");
				++pos;
				++col;
				return lh;
			}*/

			if (file[pos] == '=')
			{
				lh = Token(TokType::EqualSign, "=");
				++pos;
				++col;
				return lh;
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
				while (!eof() && (std::isalnum(static_cast<unsigned char>(file[pos])) || file[pos] == '_'))
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

		const Token& lookahead()
		{
			return lh;
		}
	};

	enum class EntryType : ipint8
	{
		Bool,
		Int,
		Float,
		String,
		Empty
	};

	struct Entry
	{
		ipstring name;
		EntryType type;
		ipstring value;

		union
		{
			ipbool boolvalue;
			ipint64 intvalue;
			ipdouble floatvalue;
		};

		Entry() :
			name(""),
			type(EntryType::Empty),
			value(""),
			boolvalue(false)
		{}

		Entry(const ipstring& _name, const ipstring& _valstr, bool _boolval) :
			name(_name),
			type(EntryType::Bool),
			value(_valstr),
			boolvalue(_boolval)
		{}

		Entry(const ipstring& _name, const ipstring& _valstr, ipint64 _intval) :
			name(_name),
			type(EntryType::Int),
			value(_valstr),
			intvalue(_intval)
		{}

		Entry(const ipstring& _name, const ipstring& _valstr, ipdouble _floatval) :
			name(_name),
			type(EntryType::Float),
			value(_valstr),
			floatvalue(_floatval)
		{}

		Entry(const ipstring& _name, const ipstring& _valstr) :
			name(_name),
			type(EntryType::String),
			value(_valstr),
			boolvalue(false)
		{}

		Entry(const Entry& other) :
			name(other.name),
			type(other.type),
			value(other.value)
		{
			switch (other.type)
			{
				case EntryType::Bool:
					boolvalue = other.boolvalue;
					break;
				case EntryType::Int:
					intvalue = other.intvalue;
					break;
				case EntryType::Float:
					floatvalue = other.floatvalue;
					break;
				default:
					break;
			}
		}

		Entry(Entry&& other) :
			name(std::move(other.name)),
			type(other.type),
			value(std::move(other.value))
		{
			switch (other.type)
			{
				case EntryType::Bool:
					boolvalue = other.boolvalue;
					break;
				case EntryType::Int:
					intvalue = other.intvalue;
					break;
				case EntryType::Float:
					floatvalue = other.floatvalue;
					break;
				default:
					break;
			}
		}

		Entry& operator=(const Entry& other)
		{
			if (this == &other)
				return *this;

			name = other.name;
			value = other.value;
			type = other.type;

			switch (type)
			{
				case EntryType::Bool:
					boolvalue = other.boolvalue;
					break;
				case EntryType::Int:
					intvalue = other.intvalue;
					break;
				case EntryType::Float:
					floatvalue = other.floatvalue;
					break;
				default:
					break;
			}

			return *this;
		}

		Entry& operator=(Entry&& other)
		{
			if (this == &other)
				return *this;

			name = std::move(other.name);
			value = std::move(other.value);
			type = other.type;
			other.type = EntryType::Empty;

			switch (type)
			{
				case EntryType::Bool:
					boolvalue = other.boolvalue;
					break;
				case EntryType::Int:
					intvalue = other.intvalue;
					break;
				case EntryType::Float:
					floatvalue = other.floatvalue;
					break;
				default:
					break;
			}

			return *this;
		}

		~Entry()
		{

		}

		std::string getValString()
		{
			switch (type)
			{
				case EntryType::Bool:
					return boolvalue ? "true" : "false";
					break;
				case EntryType::Int:
					return std::to_string(intvalue);
					break;
				case EntryType::Float:
					return std::to_string(floatvalue);
					break;
				case EntryType::String:
					return "\"" + value + "\"";
					break;
				default:
					return "";
			}
		}
	};

	//helper structure for writing
	struct Section
	{
		ipstring name;
		std::vector<Entry> entries;
		std::vector<Section> sections;
		ipuint32 idl;

		Section(const ipstring& _name, ipuint32 _idl) :
			name(_name),
			idl(_idl)
		{}
	};

	//data -------------------------------------------------------------------------------------------
	ipstring currentFile;
	Entry sentinel;
	std::unordered_map<ipstring, Entry> entries;
	YieldingSpinLock<4000> lock;
	using lckgrd = std::lock_guard<YieldingSpinLock<4000>>;

	//input validation -------------------------------------------------------------------------------

	ipbool isValidValueName(const iprstr name)
	{
		//regex: [a-zA-Z]([a-zA-Z0-9]|_)*(.[a-zA-Z]([a-zA-Z0-9]|_)*)*
		//alphabet: {a-z;A-Z;0-9;_;.}
		//States: S,I,D; I => accepting state
		constexpr ipuint8 S = 0;
		constexpr ipuint8 I = 1;
		constexpr ipuint8 D = 2;
		//grammar:
		// e := empty word
		//	S -> [a-zA-Z_]I
		//	I -> [a-zA-Z0-9_]I | .D | e
		//	D -> [a-zA-Z_]I
		//current state
		ipuint8 cs = S;
		
		for (auto c = name; *c != 0; ++c)
		{
			switch (cs)
			{
				case S:
					if (std::isalpha(static_cast<unsigned char>(*c)) || *c == '_')
					{
						cs = I;
						break;
					}
					else
						return false;
				case I:
					if (std::isalnum(static_cast<unsigned char>(*c)) || *c == '_')
					{
						cs = I;
						break;
					}
					else if (*c == '.')
					{
						cs = D;
						break;
					}
					else
						return false;
				case D:
					if (std::isalpha(static_cast<unsigned char>(*c)) || *c == '_')
					{
						cs = I;
						break;
					}
					else
						return false;
			}
		}

		return cs == I;
	}

	ipbool isValidValueName(const ipstring& name)
	{
		return isValidValueName(name.c_str());
	}

	//get/set methods --------------------------------------------------------------------------------
	const ipstring& getCurrentFile()
	{
		lckgrd lg(lock);
		return currentFile;
	}
	
	ipbool getBool(const iprstr configstring)
	{
		
		if (!isValidValueName(configstring))
			return false;
		lckgrd lg(lock);
		auto e = entries.find(configstring);
		if (e != entries.end() && e->second.type == EntryType::Bool)
		{
			return e->second.boolvalue;
		}
		else
		{
			return false;
		}
	}

	ipint64 getInt(const iprstr configstring)
	{
		if (!isValidValueName(configstring))
			return 0ll;
		lckgrd lg(lock);
		auto e = entries.find(configstring);
		if (e != entries.end() && e->second.type == EntryType::Int)
		{
			return e->second.intvalue;
		}
		else
		{
			return 0ll;
		}
	}

	ipdouble getFloat(const iprstr configstring)
	{
		if (!isValidValueName(configstring))
			return 0.0;
		lckgrd lg(lock);
		auto e = entries.find(configstring);
		if (e != entries.end() && e->second.type == EntryType::Float)
		{
			return e->second.floatvalue;
		}
		else
		{
			return 0.0;
		}
	}

	ipstring getString(const iprstr configstring)
	{
		if (!isValidValueName(configstring))
			return "";
		lckgrd lg(lock);
		auto e = entries.find(configstring);
		if (e != entries.end() && e->second.type == EntryType::String)
		{
			return e->second.value;
		}
		else
		{
			return "";
		}
	}


	ipengine::ipbool setBool(const iprstr configstring, ipbool value) //creates entry if not present
	{
		if (!isValidValueName(configstring))
			return false;
		lckgrd lg(lock);
		entries[configstring] = Entry(configstring, (value ? "true" : "false"), value);
		return true;
	}

	ipengine::ipbool setInt(const iprstr configstring, ipint64 value) //creates entry if not present
	{
		if (!isValidValueName(configstring))
			return false;
		lckgrd lg(lock);
		entries[configstring] = Entry(configstring, std::to_string(value), value);
		return true;
	}

	ipengine::ipbool setFloat(const iprstr configstring, ipdouble value) //creates entry if not present
	{
		if (!isValidValueName(configstring))
			return false;
		lckgrd lg(lock);
		entries[configstring] = Entry(configstring, std::to_string(value), value);
		return true;
	}

	ipengine::ipbool setString(const iprstr configstring, const iprstr value) //creates entry if not present
	{
		if (!isValidValueName(configstring))
			return false;
		lckgrd lg(lock);
		entries[configstring] = Entry(configstring, value);
		return true;
	}

	//load/save config files -----------------------------------------------------------------------------
	ipengine::ipbool loadConfigFile(const iprstr path)
	{
		try
		{			
			lckgrd lg(lock);
			std::ifstream fs(path, std::ios::in | std::ios::binary);
			if (!fs.is_open())
				return false;
			entries.clear();
			Scanner scan(fs);
			//some data structure
			parseFile(scan, entries);
			currentFile = path;
			return true;
		}
		catch (const std::exception& ex)
		{
			std::cerr << ex.what() << '\n';
			entries.clear();
			return false;
		}		
	}

	//parsing stuff --------------------------------------------------------------------------------------
	ipstring getSectionPrefix(const std::vector<ipstring>& seclist)
	{
		ipstring prefix = "";
		for (size_t i = 0; i < seclist.size(); i++)
		{
			prefix += seclist[i] + ".";
		}
		return prefix;
	}

	void updateSection(Scanner& scan, std::vector<ipstring>& seclist, unsigned int tabct, const ipstring& newsec)
	{
		if (tabct == seclist.size()) //new subsection in current section
		{
			seclist.push_back(newsec);
		}
		else if (tabct < seclist.size())
		{
			//pop difference, push new section
			size_t diff = seclist.size() - tabct;
			for (size_t i = 0; i < diff; i++)
			{
				seclist.pop_back();
			}
			seclist.push_back(newsec);
		}
		else
		{
			throw std::logic_error(scan.getErrorString("Bad indentation"));
		}
	}

	void updateSection(Scanner& scan, std::vector<ipstring>& seclist, unsigned int tabct)
	{
		if (tabct == seclist.size()) //do nothing
		{
			return;
		}
		else if (tabct < seclist.size())
		{
			//pop difference
			size_t diff = seclist.size() - tabct;
			for (size_t i = 0; i < diff; i++)
			{
				seclist.pop_back();
			}
		}
		else
		{
			throw std::logic_error(scan.getErrorString("Bad indentation"));
		}
	}

	void parseFile(Scanner& scan, std::unordered_map<ipstring, Entry>& map)
	{
		std::vector<ipstring> currentSection;
		while (scan.lookahead().type != TokType::EndOfFile)
		{
			parseLine(scan, map, currentSection);
		}
	}

	void parseLine(Scanner& scan, std::unordered_map<ipstring, Entry>& map, std::vector<ipstring>& currentSection)
	{
		//skip line breaks until first tab or identifier
		unsigned int tabct = 0; //count tabs to get section level
		ipstring idname;
		while (scan.lookahead().type == TokType::Tab || scan.lookahead().type == TokType::LineBreak)
		{
			if (scan.lookahead().type == TokType::Tab)
			{
				tabct++;
				scan.match(TokType::Tab);
			}
			else if (scan.lookahead().type == TokType::LineBreak) //empty line with tabs
			{
				tabct = 0;
				scan.match(TokType::LineBreak);
			}
		}

		idname = scan.lookahead().value;
		scan.match(TokType::Name);
		
		if (scan.lookahead().type == TokType::EqualSign) //value
		{
			updateSection(scan, currentSection, tabct);
			scan.match(TokType::EqualSign);
			//parse value
			parseValue(scan, map, currentSection, idname);				
			if (scan.lookahead().type == TokType::EndOfFile)
				scan.match(TokType::EndOfFile);
			else
				scan.match(TokType::LineBreak);
		}
		else //section
		{
			updateSection(scan, currentSection, tabct, idname);
			if (scan.lookahead().type == TokType::EndOfFile)
				scan.match(TokType::EndOfFile);
			else
				scan.match(TokType::LineBreak);
		}		
	}

	void parseValue(Scanner& scan, std::unordered_map<ipstring, Entry>& map, std::vector<ipstring>& currentSection, const ipstring& valname)
	{
		switch (scan.lookahead().type)
		{
			case TokType::Bool:
			{
				Entry be(getSectionPrefix(currentSection) + valname, scan.lookahead().value, scan.lookahead().boolvalue);
				map.insert(std::make_pair(be.name, std::move(be)));
				scan.match(TokType::Bool);
				break;
			}
			case TokType::Int:
			{
				Entry be(getSectionPrefix(currentSection) + valname, scan.lookahead().value, scan.lookahead().intvalue);
				map.insert(std::make_pair(be.name, std::move(be)));
				scan.match(TokType::Int);
				break;
			}
			case TokType::Float:
			{
				Entry be(getSectionPrefix(currentSection) + valname, scan.lookahead().value, scan.lookahead().floatvalue);
				map.insert(std::make_pair(be.name, std::move(be)));
				scan.match(TokType::Float);
				break;
			}
			case TokType::String:
			{
				Entry be(getSectionPrefix(currentSection) + valname, scan.lookahead().value);
				map.insert(std::make_pair(be.name, std::move(be)));
				scan.match(TokType::String);
				break;
			}
			default:

				break;
		}
	}

	//save config file ------------------------------------------------------------------------------------
	bool saveConfigFile(const char* path)
	{
		try
		{			
			lckgrd lg(lock);
			Section root("", 0);
			for (const auto& e : entries)
			{
				insertIntoWriteTree(root, e.second);
			}

			ipstring fp = path;
			if (fp == "")
				fp = currentFile;
			std::ofstream stream(fp, std::ios::trunc | std::ios::out | std::ios::binary);
			if (!stream.is_open())
				return false;

			writeConfigFile(stream, root);
			return true;			
		}
		catch (const std::exception& ex)
		{
			std::cerr << ex.what() << '\n';
			return false;
		}
	}

	void insertIntoWriteTree(Section& root, const Entry& entry)
	{
		std::vector<ipstring> epath;
		size_t stridx = 0;
		while (stridx < entry.name.size())
		{
			std::string seg("");
			while (stridx < entry.name.size() && entry.name[stridx] != '.')
				seg += entry.name[stridx++];
			epath.push_back(seg);
			++stridx;
		}

		Section* cursec = &root;
		size_t epix = 0;

		while (epix < epath.size() - 1) //find the right section or create missing ones
		{
			bool secfound = false;
			for (auto& s : cursec->sections)
			{
				if (s.name == epath[epix]) //section found
				{
					cursec = &s;
					++epix;
					secfound = true;
					break;
				}
			}
			if (!secfound) //section not found => create
			{
				cursec->sections.push_back(Section(epath[epix], epix));
				cursec = &cursec->sections.back();
				++epix;
			}
		}

		cursec->entries.push_back(entry);
	}

	void writeConfigFile(std::ofstream& stream, Section& sec)
	{
		std::vector<Section*> stack;
		stack.push_back(&sec);
		while (!stack.empty())
		{
			//process section:
			auto s = stack.back();
			stack.pop_back();
			//- write section
			if (s != &sec) //do not write root secion
			{
				for (unsigned int i = 0; i < s->idl; i++)
					stream << '\t';
				stream << s->name << '\n';
			}
			//- write entries
			for (size_t i = 0; i < s->entries.size(); ++i)
			{
				if(s != &sec)
					for (unsigned int i = 0; i < s->idl + 1; i++)
						stream << '\t';
				auto dtpos = s->entries[i].name.rfind(".");
				ipstring sname = (dtpos != ipstring::npos ? s->entries[i].name.substr(dtpos + 1, ipstring::npos) : s->entries[i].name);
				stream << sname << " = " << s->entries[i].getValString() << '\n';
			}
			//- push sections onto stack
			for (size_t i = 0; i < s->sections.size(); ++i)
				stack.push_back(&s->sections[s->sections.size() - 1 - i]);
		}
	}

};

// interface ----------------------------------------------------------------------------------------------

ipengine::ConfigManager::ConfigManager() :
	m_impl(new ConfigManager::ConfigImpl())
{}

ipengine::ConfigManager::~ConfigManager()
{
	if (m_impl)
		delete m_impl;
}

ipengine::ipbool ipengine::ConfigManager::loadConfigFile(const iprstr path)
{
	return m_impl->loadConfigFile(path);
}

ipengine::ipbool ipengine::ConfigManager::saveConfigFile(const iprstr path)
{
	return m_impl->saveConfigFile(path);
}

ipengine::ipstring ipengine::ConfigManager::getCurrentFile()
{
	return m_impl->getCurrentFile();
}

ipengine::ipbool ipengine::ConfigManager::getBool(const iprstr configstring)
{
	return m_impl->getBool(configstring);
}

ipengine::ipint64 ipengine::ConfigManager::getInt(const iprstr configstring)
{
	return m_impl->getInt(configstring);
}

ipengine::ipdouble ipengine::ConfigManager::getFloat(const iprstr configstring)
{
	return m_impl->getFloat(configstring);
}

ipengine::ipstring ipengine::ConfigManager::getString(const iprstr configstring)
{
	return m_impl->getString(configstring);
}

ipengine::ipbool ipengine::ConfigManager::setBool(const iprstr configstring, ipbool value)
{
	return m_impl->setBool(configstring, value);
}

ipengine::ipbool ipengine::ConfigManager::setInt(const iprstr configstring, ipint64 value)
{
	return m_impl->setInt(configstring, value);
}

ipengine::ipbool ipengine::ConfigManager::setFloat(const iprstr configstring, ipdouble value)
{
	return m_impl->setFloat(configstring, value);
}

ipengine::ipbool ipengine::ConfigManager::setString(const iprstr configstring, const iprstr value)
{
	return m_impl->setString(configstring, value);
}