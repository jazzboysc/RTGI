//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Terminal.h"
#include <cstdarg>

using namespace RTGI;

enum FG_COLOR
{
    FG_RED = FOREGROUND_RED,
    FG_GREEN = FOREGROUND_GREEN,
    FG_BLUE = FOREGROUND_BLUE,
    FG_YELLOW = FG_RED | FG_GREEN,
    FG_MAGENTA = FG_RED | FG_BLUE,
    FG_CYAN = FG_BLUE | FG_GREEN,
    FG_WHITE = FG_RED | FG_GREEN | FG_BLUE,
    FG_BLACK = 0,
    FG_LIGHT_RED = FG_RED | FOREGROUND_INTENSITY,
    FG_LIGHT_GREEN = FG_GREEN | FOREGROUND_INTENSITY,
    FG_LIGHT_BLUE = FG_BLUE | FOREGROUND_INTENSITY,
    FG_LIGHT_YELLOW = FG_YELLOW | FOREGROUND_INTENSITY
};

//----------------------------------------------------------------------------
BOOL SetTextColor(const FG_COLOR color)
{
	HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hConsole, &mode);
	SetConsoleMode(hConsole, mode & ~ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT);

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(handle, &info);
    info.wAttributes &= ~FG_WHITE; // Turn off existing text colors
    info.wAttributes |= color;

    return SetConsoleTextAttribute(handle, info.wAttributes);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
Terminal::Terminal()
{
}
//----------------------------------------------------------------------------
Terminal::~Terminal()
{
}
//----------------------------------------------------------------------------
void Terminal::Output(OutputColor color, const char* format, ...)
{
    switch( color )
    {
    case Terminal::OC_Success:
        SetTextColor(FG_LIGHT_GREEN);
        break;

    case Terminal::OC_Error:
        SetTextColor(FG_LIGHT_RED);
        break;

    case Terminal::OC_Warning_Level1:
    case Terminal::OC_Warning_Level2:
    case Terminal::OC_Warning_Level3:
    case Terminal::OC_Warning_Level4:
        SetTextColor(FG_LIGHT_YELLOW);
        break;

    default:
        break;
    }

    va_list arg;
    va_start(arg, format);
    vfprintf(stdout, format, arg);
    va_end(arg);
}
//----------------------------------------------------------------------------