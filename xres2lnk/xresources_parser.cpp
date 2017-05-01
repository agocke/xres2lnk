#include <iostream>
#include <regex>
#include <string>
#include <sstream>
#include <assert.h>
#include <memory>
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

ColorInfo parse_xresources_file(std::string contents)
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

            if (colorNumber < 0 || colorNumber > 15)
            {
                std::cerr << "Color id specified in '" << line << "' is invalid. Skipping..." << std::endl;
            }
            else
            {
                colorInfo.NumberedColors[colorNumber] = color;
            }
        }
    }
    
    return colorInfo;
}