//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "ShaderUniform.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ShaderUniform::ShaderUniform()
{
    mType = UT_Unknown;
    mUniformHandle = 0;
}
//----------------------------------------------------------------------------
ShaderUniform::~ShaderUniform()
{
    delete mUniformHandle;
}
//----------------------------------------------------------------------------