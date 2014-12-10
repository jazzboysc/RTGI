//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_ShaderUniform_H
#define RTGI_ShaderUniform_H

#include "GPUResource.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/09/2014
//----------------------------------------------------------------------------
class ShaderUniform
{
public:
    ShaderUniform();
    ~ShaderUniform();

    enum UniformType
    {
        UT_Unknown = 0,
        UT_SystemPerFrame,
        UT_SystemPerGroup,
        UT_SystemPerObject,
        UT_UserPerFrame,
        UT_UserPerGroup,
        UT_UserPerObject,
        UT_Max
    };

public:
    std::string mName;
    UniformType mType;
    ShaderUniformHandle* mUniformHandle;
};

typedef RefPointer<ShaderUniform> ShaderUniformPtr;

}

#endif