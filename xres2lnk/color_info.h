#pragma once
#include <stdint.h>
#include <Windows.h>

struct Color
{
    uint8_t Red = 0;
    uint8_t Blue = 0;
    uint8_t Green = 0;

public:
    COLORREF to_color_ref()
    {
        return RGB(Red, Blue, Green);
    }
};

struct ColorInfo
{
    Color ForegroundColor = Color();
    Color BackgroundColor = Color();
    Color Black = Color();
    Color Red = Color();
    Color Green = Color();
    Color Yellow = Color();
    Color Blue = Color();
    Color Magenta = Color();
    Color Cyan = Color();
    Color White = Color();
    Color BrightBlack = Color();
    Color BrightRed = Color();
    Color BrightGreen = Color();
    Color BrightYellow = Color();
    Color BrightBlue = Color();
    Color BrightMagenta = Color();
    Color BrightCyan = Color();
    Color BrightWhite = Color();
};