#include <iostream>
#include <string>
#include "stb_image.h"
#include "stb_image_write.h"
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>
#include "Scanner.h"

enum class OutFileFormat
{
	png,
	jpg,
	bmp,
	tga
};

struct chdesc
{
	size_t i_idx;
	size_t c_idx;
};

struct imdesc
{
	std::string path;	
	int channels;
	int width;
	int height;
	OutFileFormat format;
	unsigned char* data;

	imdesc() :
		path(""),
		channels(0),
		width(0),
		height(0),
		data(nullptr)
	{}

	imdesc(const std::string& p, int c, int w, int h, unsigned char* d, OutFileFormat f = OutFileFormat::png) :
		path(p),
		channels(c),
		width(w),
		height(h),
		data(d),
		format(f)
	{}

	imdesc(const imdesc& other) = delete;
	imdesc& operator=(const imdesc& other) = delete;

	imdesc(imdesc&& other) :
		path(std::move(other.path)),
		channels(other.channels),
		width(other.width),
		height(other.height),
		data(other.data),
		format(other.format)
	{
		other.data = nullptr;
		other.width = 0;
		other.height = 0;
		other.channels = 0;
	}

	imdesc& operator=(imdesc&& other)
	{
		if (this == &other)
			return *this;

		path = std::move(other.path);
		channels = other.channels;
		width = other.width;
		height = other.height;
		data = other.data;
		format = other.format;
		
		other.data = nullptr;
		other.width = 0;
		other.height = 0;
		other.channels = 0;

		return *this;
	}

	~imdesc()
	{
		if (data)
			stbi_image_free(data);
	}
};

struct wgterm
{	
	bool constval;	
	float weight;
	union
	{
		float val;
		chdesc channel;
	};

	wgterm(float w, const chdesc& c) :
		weight(w),
		channel(c),
		constval(false)
	{}

	wgterm(float w, float v) :
		weight(w),
		val(v),
		constval(true)
	{}

	wgterm(const wgterm& other) :
		constval(other.constval),
		weight(other.weight)
	{
		if (other.constval)
			val = other.val;
		else
			channel = other.channel;
	}

	wgterm& operator=(const wgterm& other)
	{
		if (this == &other)
			return *this;

		weight = other.weight;
		constval = other.constval;

		if (other.constval)
			val = other.val;
		else
			channel = other.channel;

		return *this;
	}
};

struct rule
{
	chdesc target;
	std::vector<wgterm> sources;
};

void interpretLine(Scanner& scan, std::vector<imdesc>& input, std::vector<imdesc>& output, std::vector<rule>& rules);
void parseIn(Scanner& scan, std::vector<imdesc>& input, std::vector<imdesc>& output, std::vector<rule>& rules);
void parseOut(Scanner& scan, std::vector<imdesc>& input, std::vector<imdesc>& output, std::vector<rule>& rules);
void parseRule(Scanner& scan, std::vector<imdesc>& input, std::vector<imdesc>& output, std::vector<rule>& rules);
void parsePack(Scanner& scan, std::vector<imdesc>& input, std::vector<imdesc>& output, std::vector<rule>& rules);
void parseClear(Scanner& scan, std::vector<imdesc>& input, std::vector<imdesc>& output, std::vector<rule>& rules);
void parseExit(Scanner& scan, std::vector<imdesc>& input, std::vector<imdesc>& output, std::vector<rule>& rules);
void parseTarget(Scanner& scan, rule& r);
void parseSources(Scanner& scan, rule& r);
bool validateRule(const rule& r, std::vector<imdesc>& input, std::vector<imdesc>& output);
void printStat(const std::vector<imdesc>& input, const std::vector<imdesc>& output, const std::vector<rule>& rules);

void addInputFile(std::vector<imdesc>& input, const std::string& path);
void addOutputFile(std::vector<imdesc>& output, const std::string& path, size_t channels);

bool sexit = false;

int main()
{
	std::vector<imdesc> input;
	std::vector<imdesc> output;
	std::vector<rule> rules;

	std::cout <<
		R"#(
TexPack texture mixing tool
---------------------------

Commands:
    in        load an input file
    out       specify an output file with the desired channel count
    rule      specify an output rule for one channel of one output file
    pack      process the rules and write output files
    clear     clear input, output and rule lists
    exit      exit

Workflow is as follows:
    1.    Load one or more input files
    2.    Specify one or more output files
    3.    Specify a rule for each channel of the output files
    4.    Process the rules and write the output files via the "pack" command

Syntax:
<> => required
[] => optional
.. => 0..n times the preceding expression
(...|...) => choose one of the options

    in "<path>"
        path                Path to a valid input file.
                            Supported file formats: .png, .jpg, .tga, .bmp

    out "<path>" <channelcount>
        path                Path to a valid output file. File ending defines the output format.
                            Supported output formats: .png, .jpg, .tga, .bmp
                            If the file ending is not one of {.png, .jpg, .jpeg, .tga, .bmp},
                            the output format defaults to .png
        channelcount        Number of desired channels in the output file.
                            Must be a number in the range [1, 4].

    rule <outindex>:<channel> = ([channelweight *] <inindex>:<channel> | <constval>) [+ ([channelweight *] <inindex>:<channel> | <constval>)]..
        Each pixel value of an output channel is calculated according to the rule targeting that channel.
        A rule defines a weighted sum of constant values and/or weighted input channel values.
        
        outindex            Zero-based index of the targeted output file. Must be a valid index
                            from the output file list.
        channel             Zero-based channel index. Must be a valid index
                            for the selected file.
        inindex             Zero-based index of an input file. Must be a valid index
                            from the input file list.
        channelweight       Floating point value wich is multiplied with the respective input channel value.
                            Defaults to 1.0 if this value is omitted.
        constval            Floating point value wich is handled as a constant for the whole image.

----------------------------
	
)#";

	while (!sexit)
	{
		std::cout << ">";
		try
		{
			std::string line;
			std::getline(std::cin, line);
			std::stringstream sstrm(line);
			Scanner scan(sstrm);

			interpretLine(scan, input, output, rules);
			printStat(input, output, rules);
		}
		catch (std::exception& ex)
		{
			std::cout << "Error: " << ex.what() << "\n";
		}
	}
}

void printStat(const std::vector<imdesc>& input, const std::vector<imdesc>& output, const std::vector<rule>& rules)
{
	std::cout << "-------------------------------------------------------------------------\n";
	std::cout << "-------- input files:\n\n";
	for (size_t i = 0; i < input.size(); ++i)
	{
		std::cout << i << "\twidth: " << input[i].width << "\theight: " << input[i].height << "\tchannels: " << input[i].channels << "\n";
		std::cout << "\tpath: " << input[i].path << "\n\n";
	}
	std::cout << "-------- output files:\n\n";
	for (size_t i = 0; i < output.size(); ++i)
	{
	std::cout << i << "\tchannels: " << output[i].channels << "\n";
	std::cout << "\tpath: " << output[i].path << "\n\n";
	}
	std::cout << "-------- rules:\n\n";
	for (size_t i = 0; i < rules.size(); ++i)
	{
		std::cout << rules[i].target.i_idx << ":" << rules[i].target.c_idx << "\t->\t";
		for (size_t k = 0; k < rules[i].sources.size(); ++k)
		{
			if (rules[i].sources[k].constval)
			{
				std::cout << std::setprecision(4) << rules[i].sources[k].weight <<
					" * " << rules[i].sources[k].val <<
					(k == (rules[i].sources.size() - 1) ? "" : " + ");
			}
			else
			{
				std::cout << std::setprecision(4) << rules[i].sources[k].weight <<
					" * " << rules[i].sources[k].channel.i_idx << ":" << rules[i].sources[k].channel.c_idx <<
					(k == (rules[i].sources.size() - 1) ? "" : " + ");
			}
		}
		std::cout << "\n\n";
	}
}

void interpretLine(Scanner& scan, std::vector<imdesc>& input, std::vector<imdesc>& output, std::vector<rule>& rules)
{
	switch (scan.lookahead().type)
	{
		case TokType::In:
			scan.match(TokType::In);
			parseIn(scan, input, output, rules);
			break;
		case TokType::Out:
			scan.match(TokType::Out);
			parseOut(scan, input, output, rules);
			break;
		case TokType::Rule:
			scan.match(TokType::Rule);
			parseRule(scan, input, output, rules);
			break;
		case TokType::Pack:
			scan.match(TokType::Pack);
			parsePack(scan, input, output, rules);
			break;
		case TokType::Clear:
			scan.match(TokType::Clear);
			parseClear(scan, input, output, rules);
			break;
		case TokType::Exit:
			scan.match(TokType::Exit);
			parseExit(scan, input, output, rules);
			break;
		default:
			throw std::logic_error("Unknown command '" + scan.lookahead().value + "'");
			break;
	}
}

void parseIn(Scanner & scan, std::vector<imdesc>& input, std::vector<imdesc>& output, std::vector<rule>& rules)
{
	std::string path = scan.lookahead().value;
	scan.match(TokType::String);
	addInputFile(input, path);
}

void parseOut(Scanner & scan, std::vector<imdesc>& input, std::vector<imdesc>& output, std::vector<rule>& rules)
{
	std::string path = scan.lookahead().value;
	scan.match(TokType::String);
	int ch = scan.lookahead().intvalue;
	scan.match(TokType::Int);
	addOutputFile(output, path, static_cast<size_t>(ch));
}

void parseRule(Scanner & scan, std::vector<imdesc>& input, std::vector<imdesc>& output, std::vector<rule>& rules)
{
	rule r;
	parseTarget(scan, r);
	scan.match(TokType::EqualSign);
	parseSources(scan, r);
	if (validateRule(r, input, output))
		rules.push_back(r);
	else
		throw std::logic_error("Invalid rule.");
}

float btof(unsigned char b)
{
	return static_cast<float>(b) / 255.0f;
}

unsigned char ftob(float f)
{
	return static_cast<unsigned char>(f * 255.0f);
}

float clamp(float v, float min, float max)
{
	if (v < min)
		return min;
	else if (v > max)
		return max;
	else
		return v;
}

size_t i2d(size_t x, size_t y, size_t width)
{
	return y * width + x;
}

void parsePack(Scanner & scan, std::vector<imdesc>& input, std::vector<imdesc>& output, std::vector<rule>& rules)
{
	std::cout << "Packing output files...\n";

	//get smallest size
	size_t minwidth = std::numeric_limits<size_t>::max();
	size_t minheight = std::numeric_limits<size_t>::max();

	for (auto& i : input)
	{
		minwidth = std::min(minwidth, static_cast<size_t>(i.width));
		minheight = std::min(minheight, static_cast<size_t>(i.height));
	}

	for (size_t oi = 0; oi < output.size(); ++oi)
	{
		std::cout << "Processing " << output[oi].path << "...\n";
		//allocate image
		size_t outsize = minwidth * minheight * output[oi].channels;
		output[oi].data = new unsigned char[outsize];

		//create per pixel rule set for this output
		std::vector<rule> or;
		or.reserve(output[oi].channels);
		for (size_t ori = 0; ori < output[oi].channels; ++ori)
		{
			or.push_back(rule{
				chdesc{
					oi,
					ori
				},
				{
					wgterm{
						1.0f,
						0.0f
					}
				}
			});
		}

		for (size_t oci = 0; oci < output[oi].channels; ++oci)
		{
			for (auto& r : rules)
			{
				if (r.target.i_idx == oi && r.target.c_idx == oci)
				{
					or[oci] = r;
					break;
				}
			}
		}

		//fill output buffer
		for (size_t piy = 0; piy < minheight; ++piy)
		{
			for (size_t pix = 0; pix < minwidth; ++pix)
			{
				for (size_t c = 0; c < output[oi].channels; ++c)
				{
					float pval = 0.0f;
					for (auto& t : or[c].sources)
					{
						if (t.constval)
						{
							pval += t.weight * t.val;
						}
						else
						{
							pval += t.weight * btof(
								input[t.channel.i_idx]
								.data[
									i2d(pix * input[t.channel.i_idx].channels, piy, input[t.channel.i_idx].width * input[t.channel.i_idx].channels)
										+ t.channel.c_idx]);
						}
					}
					output[oi].data[i2d(pix * output[oi].channels, piy, minwidth * output[oi].channels) + c] = ftob(clamp(pval, 0.0f, 1.0f));
				}
			}
		}
		//write image
		std::cout << "Writing " << output[oi].path << "...\n";

		bool success;
		switch (output[oi].format)
		{
			case OutFileFormat::png:
				success = stbi_write_png(output[oi].path.c_str(), minwidth, minheight, static_cast<int>(output[oi].channels), output[oi].data, 0);
				break;
			case OutFileFormat::jpg:
				success = stbi_write_jpg(output[oi].path.c_str(), minwidth, minheight, static_cast<int>(output[oi].channels), output[oi].data, 100);
				break;
			case OutFileFormat::tga:
				success = stbi_write_tga(output[oi].path.c_str(), minwidth, minheight, static_cast<int>(output[oi].channels), output[oi].data);
				break;
			case OutFileFormat::bmp:
				success = stbi_write_bmp(output[oi].path.c_str(), minwidth, minheight, static_cast<int>(output[oi].channels), output[oi].data);
				break;
		}
		if (!success)
			throw std::logic_error("Error writing file: " + output[oi].path);

		delete[] output[oi].data;
		output[oi].data = nullptr;
	}
}

void parseClear(Scanner & scan, std::vector<imdesc>& input, std::vector<imdesc>& output, std::vector<rule>& rules)
{
	if (scan.lookahead().type == TokType::Rule)
	{
		scan.match(TokType::Rule);
		rules.clear();
	}
	else if (scan.lookahead().type == TokType::EndOfFile)
	{
		input.clear();
		output.clear();
		rules.clear();
	}
	else
	{
		throw std::logic_error("Invalid clear command");
	}
}

void parseExit(Scanner & scan, std::vector<imdesc>& input, std::vector<imdesc>& output, std::vector<rule>& rules)
{
	sexit = true;
}

void parseTarget(Scanner & scan, rule & r)
{
	size_t i, c;
	i = static_cast<size_t>(scan.lookahead().intvalue);
	scan.match(TokType::Int).match(TokType::Colon);
	c = static_cast<size_t>(scan.lookahead().intvalue);
	scan.match(TokType::Int);
	r.target = chdesc{i, c};
}

void parseSources(Scanner & scan, rule & r)
{
	std::vector<wgterm> weightedterms;
	while (scan.lookahead().type != TokType::EndOfFile)
	{
		if (scan.lookahead().type == TokType::Float)
		{
			float coeff = scan.lookahead().floatvalue;
			scan.match(TokType::Float);
			if (scan.lookahead().type == TokType::Mul)
			{
				scan.match(TokType::Mul);
				if (scan.lookahead().type == TokType::Int)
				{
					size_t i, c;
					i = static_cast<size_t>(scan.lookahead().intvalue);
					scan.match(TokType::Int).match(TokType::Colon);
					c = scan.lookahead().intvalue;
					scan.match(TokType::Int);

					weightedterms.push_back(
						wgterm{
							coeff,
							chdesc{i, c}
						}
					);
				}
				else if (scan.lookahead().type == TokType::Float)
				{
					float val = scan.lookahead().floatvalue;
					scan.match(TokType::Float);
					weightedterms.push_back(
						wgterm{
							coeff,
							val
						}
					);
				}
				else
				{
					throw std::logic_error("Invalid rule syntax, unexpected token '" + scan.lookahead().value + "'");
				}
			}
			else if (scan.lookahead().type == TokType::Plus || scan.lookahead().type == TokType::EndOfFile)
			{
				weightedterms.push_back(
						wgterm{
						1.0f,
						coeff
					}
				);
			}
			else
			{
				throw std::logic_error("Invalid rule syntax, unexpected token '" + scan.lookahead().value + "'");
			}
		}
		else if (scan.lookahead().type == TokType::Int)
		{
			size_t i, c;
			i = static_cast<size_t>(scan.lookahead().intvalue);
			scan.match(TokType::Int).match(TokType::Colon);
			c = scan.lookahead().intvalue;
			scan.match(TokType::Int);
			weightedterms.push_back(
				wgterm{
					1.0f,
					chdesc{i, c}
				}
			);
		}
		else
		{
			throw std::logic_error("Invalid rule syntax, unexpected token '" + scan.lookahead().value + "'");
		}
		if (scan.lookahead().type == TokType::EndOfFile) break;
		else scan.match(TokType::Plus);
	}
	r.sources = weightedterms;
}

bool validateRule(const rule & r, std::vector<imdesc>& input, std::vector<imdesc>& output)
{
	if (r.target.i_idx >= 0 && r.target.i_idx < output.size() &&
		r.target.c_idx >= 0 && r.target.c_idx < output[r.target.i_idx].channels)
	{
		bool sourcesvalid = true;

		for (auto& s : r.sources)
		{
			if (!s.constval)
			{
				if (s.channel.i_idx < 0 || s.channel.i_idx >= input.size())
				{
					sourcesvalid = false;
					break;
				}
				if (s.channel.c_idx < 0 || s.channel.c_idx >= input[s.channel.i_idx].channels)
				{
					sourcesvalid = false;
					break;
				}
			}
		}
		return sourcesvalid;
	}
	return false;
}

OutFileFormat getImageFormat(const std::string& path)
{
	std::string fileending;
	for (auto it = path.rbegin(); it != path.rend(); ++it)
	{
		if (*it != '.')
		{
			fileending += std::tolower(static_cast<unsigned char>(*it));
		}
		else
		{
			break;
		}
	}

	if (fileending == "png")
		return OutFileFormat::png;
	else if (fileending == "jpeg" || fileending == "jpg")
		return OutFileFormat::jpg;
	else if (fileending == "tga")
		return OutFileFormat::tga;
	else if (fileending == "bmp")
		return OutFileFormat::bmp;
	else
		return OutFileFormat::png;
}

void addInputFile(std::vector<imdesc>& input, const std::string & path)
{
	imdesc im;
	im.path = path;
	std::cout << "Reading image file...\n";
	im.data = stbi_load(path.c_str(), &im.width, &im.height, &im.channels, 0);
	im.format = getImageFormat(path);
	if (im.data)
	{
		input.push_back(std::move(im));
	}
	else
	{
		throw std::logic_error("Error reading image file.");
	}
}

void addOutputFile(std::vector<imdesc>& output, const std::string & path, size_t channels)
{
	if (channels > 0 && channels <= 4)
	{
		imdesc im;
		im.channels = channels;
		im.path = path;
		im.width = 0;
		im.height = 0;
		im.data = nullptr;
		im.format = getImageFormat(path);
		output.push_back(std::move(im));
	}
	else
	{
		throw std::exception("Invalid channel count for output image.");
	}
}