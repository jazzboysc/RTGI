//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_PassBase_H
#define RTGI_PassBase_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "ShaderProgram.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 08/09/2014
//----------------------------------------------------------------------------
class PassBase : public RefObject
{
public:
    PassBase(const ShaderProgramInfo& programInfo);
    PassBase(ShaderProgram* shaderProgram);
    virtual ~PassBase();

    ShaderProgram* GetShaderProgram();
    void CreateDeviceResource();

protected:
    ShaderProgramPtr mShaderProgram;
};

typedef RefPointer<PassBase> PassBasePtr;

}

#endif