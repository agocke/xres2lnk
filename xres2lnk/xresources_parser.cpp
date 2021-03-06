#include <iostream>
#include <regex>
#include <string>
#include <sstream>
#include <assert.h>
#include <memory>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <utility>
#include "color_info.h"

static Color colorspec_to_color(std::string& colorSpec)
{
    assert(colorSpec.length() == 6);
    Color color;
    color.Red = stoi(colorSpec.substr(0, 2), NULL, /* hex */16);
    color.Blue = stoi(colorSpec.substr(2, 2), NULL, /* hex */16);
    color.Green = stoi(colorSpec.substr(4, 2), NULL, /* hex */16);
    return color;
}

std::string preprocess_xresources_file(const std::string &contents)
{
	std::istringstream iss(contents);

	// map with
	//   key is string containing identifier
	//   value is pair of compiled regex for matching the identifier and the replacement string
	// using a map instead of a vector of pairs allows for detection of collision among defines
	std::map<std::string, std::pair<std::regex, std::string>> replacements;
	const std::regex define_re(
		R"(^#define\s+([_a-zA-Z][_0-9a-zA-Z]*)\s+(.*))"
	);
	std::ostringstream oss;
	for (std::string line; std::getline(iss, line);)
	{
		std::smatch define_match;
		std::regex_match(line, define_match, define_re);
		if (define_match.size() == 3)
		{
			std::string identifier = define_match[1].str();
			if (replacements.find(identifier) == replacements.end())
			{
				replacements[identifier] = std::make_pair(std::regex(identifier), define_match[2].str());
			}
			else
			{
				std::cerr << "warning: multiple defines for " << identifier << std::endl;
			}
			oss << line << std::endl;
		}
		else
		{
			if (!line.empty())
			{
				// lazy replacement algorithm
				for (auto &p : replacements)
				{
					line = std::regex_replace(line, p.second.first, p.second.second);
				}
			}
			oss << line << std::endl;
		}
	}

	return oss.str();
}

ColorInfo parse_xresources_file(const std::string &contents)
{
    std::stringstream xres(contents, std::stringstream::ios_base::in);
    ColorInfo colorInfo = ColorInfo();
    for (std::string line; std::getline(xres, line);)
    {
        if (line.empty())
        {
            continue;
        }

        const std::regex colorLine(
            R"((?:rxvt|term|^\s*|^\s*\*))" // Optional term specifier
            R"((?:\*|\.)?)" // Separator 
            R"((foreground|background|color\d{1,2})\s*:\s*)" // Color id
            R"(#([a-f0-9]{6}))", // Color specifier
            std::regex_constants::icase);

        std::smatch matchResult;
        if (!std::regex_search(line, matchResult, colorLine))
        {
            continue;
        }

        auto iter = matchResult.begin();
        iter++;
        auto colorId = iter->str();
        iter++;
        Color color = colorspec_to_color(iter->str());

        if (colorId == "foreground")
        {
            colorInfo.ForegroundColor = color;
        }
        else if (colorId == "background")
        {
            colorInfo.BackgroundColor = color;
        }
        else
        {
            const size_t colorlen = sizeof("color") - 1;
            auto colorNumber = stoi(colorId.substr(colorlen, colorId.length() - colorlen));

            switch (colorNumber)
            {
            case 0:
                colorInfo.Black = color;
                break;
            case 1:
                colorInfo.Red = color;
                break;
            case 2:
                colorInfo.Green = color;
                break;
            case 3:
                colorInfo.Yellow = color;
                break;
            case 4:
                colorInfo.Blue = color;
                break;
            case 5:
                colorInfo.Magenta = color;
                break;
            case 6:
                colorInfo.Cyan = color;
                break;
            case 7:
                colorInfo.White = color;
                break;
            case 8:
                colorInfo.BrightBlack = color;
                break;
            case 9:
                colorInfo.BrightRed = color;
                break;
            case 10:
                colorInfo.BrightGreen = color;
                break;
            case 11:
                colorInfo.BrightYellow = color;
                break;
            case 12:
                colorInfo.BrightBlue = color;
                break;
            case 13:
                colorInfo.BrightMagenta = color;
                break;
            case 14:
                colorInfo.BrightCyan = color;
                break;
            case 15:
                colorInfo.BrightWhite = color;
                break;
            default:
                std::cerr << "Color id specified in '" << line << "' is invalid. Skipping..." << std::endl;
                break;
            }
        }
    }
    
    return colorInfo;
}