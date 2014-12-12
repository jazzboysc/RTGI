//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Terminal_H
#define RTGI_Terminal_H

#include <Windows.h>

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/12/2014
//----------------------------------------------------------------------------
class Terminal
{
public:
    Terminal();
    ~Terminal();

    enum OutputColor
    {
        OC_Success = 0,
        OC_Error,
        OC_Warning_Level1,
        OC_Warning_Level2,
        OC_Warning_Level3,
        OC_Warning_Level4
    };

    static void Output(OutputColor color, const char* format, ...);
};

}

#endif