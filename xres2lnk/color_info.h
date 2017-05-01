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
    Color NumberedColors[16];
    Color ForegroundColor = Color();
    Color BackgroundColor = Color();
};