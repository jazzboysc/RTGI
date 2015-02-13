//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "GPUTimer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
GPUTimer::GPUTimer()
    :
    mTimer(0)
{
    mStarted = false;
}
//----------------------------------------------------------------------------
GPUTimer::~GPUTimer()
{
    glDeleteQueries(1, &mTimer);
}
//----------------------------------------------------------------------------
void GPUTimer::CreateDeviceResource()
{
    glGenQueries(1, &mTimer);
}
//----------------------------------------------------------------------------
void GPUTimer::Start()
{
    glBeginQuery(GL_TIME_ELAPSED, mTimer);
    mStarted = true;
}
//----------------------------------------------------------------------------
void GPUTimer::Stop()
{
    glEndQuery(GL_TIME_ELAPSED);
}
//----------------------------------------------------------------------------
double GPUTimer::GetTimeElapsed()
{
    if( !mStarted )
    {
        return 0.0;
    }

    GLuint64 timeElapsed = 0;
    double res = 0.0;

    GLint available = 0;
    while( !available )
    {
        glGetQueryObjectiv(mTimer, GL_QUERY_RESULT_AVAILABLE, &available);
    }
    glGetQueryObjectui64v(mTimer, GL_QUERY_RESULT, &timeElapsed);

    res = double(timeElapsed) / 1000000.0;
    return res;
}
//----------------------------------------------------------------------------