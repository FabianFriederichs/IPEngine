#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>

enum class TokType
{
	String,
	Float,
	Int,
	EqualSign,
	Invalid,
	EndOfFile,
	Colon,
	Plus,
	Mul,
	In,
	Out,
	Rule,
	Pack,
	Clear,
	Exit
};

struct Token
{
	Token() :
		type(TokType::Invalid),
		value(""),
		intvalue(0)
	{}

	Token(TokType _type, const std::string& sval) :
		type(_type),
		value(sval),
		intvalue(0)
	{}

	Token(TokType _type, const std::string& sval, int intval) :
		type(_type),
		value(sval),
		intvalue(intval)
	{}

	Token(TokType _type, const std::string& sval, float floatval) :
		type(_type),
		value(sval),
		floatvalue(floatval)
	{}

	TokType type;
	std::string value;
	union
	{
		int intvalue;
		float floatvalue;
	};
};

struct Scanner
{
	std::string file;
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

	std::string getErrorString(const std::string& msg)
	{
		std::string estr = "Command line parsing error at line: ";
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

		if (file[pos] == ' ')
		{
			++pos;
			while (!eof() && file[pos] == ' ')
			{
				++pos;
			}
		}

		if (file[pos] == '.' || file[pos] == '-' || std::isdigit(static_cast<unsigned char>(file[pos])))
		{
			//test for float or integer, maybe include e notation
			std::string num("");
			std::string epart("");
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

			int rawnum = 0;
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
				float res = (sign ? -1.0 : 1.0) * static_cast<float>(rawnum) * std::pow(10.0, static_cast<float>(pot));
				lh = Token(TokType::Float, num + epart, res);
				return lh;
			}
			else if (!dot && num.length() >= 1)
			{
				int res = (sign ? -1 : 1) * rawnum * static_cast<int>(std::pow(int(10), static_cast<int>(pot)));
				lh = Token(TokType::Int, num + epart, res);
				return lh;
			}
			else
			{
				throw std::logic_error(getErrorString("Wrong number format"));
			}
		}

		if (file[pos] == '=')
		{
			lh = Token(TokType::EqualSign, "=");
			++pos;
			++col;
			return lh;
		}

		if (file[pos] == ':')
		{
			lh = Token(TokType::Colon, ":");
			++pos;
			++col;
			return lh;
		}

		if (file[pos] == '+')
		{
			lh = Token(TokType::Plus, "+");
			++pos;
			++col;
			return lh;
		}

		if (file[pos] == '*')
		{
			lh = Token(TokType::Mul, "*");
			++pos;
			++col;
			return lh;
		}

		if (file[pos] == '"')
		{
			std::string str = "";
			std::string::value_type c;
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

		if (std::isalpha(static_cast<unsigned char>(file[pos])))
		{
			std::string cmdname = "";
			cmdname += file[pos++];
			++col;
			while (!eof() && std::isalpha(static_cast<unsigned char>(file[pos])))
			{
				cmdname += file[pos++];
				++col;
			}
			//check for keywords
			if (cmdname == "in")
				lh = Token(TokType::In, "<in>");
			else if (cmdname == "out")
				lh = Token(TokType::Out, "<out>");
			else if (cmdname == "rule")
				lh = Token(TokType::Rule, "<rule>");
			else if (cmdname == "pack")
				lh = Token(TokType::Pack, "<pack>");
			else if (cmdname == "clear")
				lh = Token(TokType::Clear, "<clear>");
			else if (cmdname == "exit")
				lh = Token(TokType::Exit, "<exit>");
			else
				throw std::logic_error("Unknown command '" + cmdname + "'");
			
			return lh;
		}

		std::string estr = "Unexpected '";
		estr += file[pos];
		estr += "'";
		throw std::logic_error(getErrorString(estr));
	}

	Scanner& match(TokType mt)
	{
		if (lh.type == mt)
		{
			next();
			return *this;
		}
		std::string estr = "Unexpected '" + lh.value + "'";
		throw std::logic_error(getErrorString(estr));
	}

	const Token& lookahead()
	{
		return lh;
	}
};

#endif
