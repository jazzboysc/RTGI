//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_GPUTimer_H
#define RTGI_GPUTimer_H

#include "RefObject.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 10/27/2014
//----------------------------------------------------------------------------
class GPUTimer : public RefObject
{
public:
    GPUTimer();
    ~GPUTimer();

    void CreateDeviceResource();
    void Start();
    void Stop();
    double GetTimeElapsed();

private:
    GLuint mTimer;
    bool mStarted;
};

typedef RefPointer<GPUTimer> GPUTimerPtr;

}

#endif