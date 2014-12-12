//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "ShaderUniform.h"
#include "GPUDevice.h"

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
void ShaderUniform::SetValue(int value)
{
    GPU_DEVICE_FUNC_SetUniformValueInt(*this, value);
}
//----------------------------------------------------------------------------
void ShaderUniform::SetValue(float value)
{
    GPU_DEVICE_FUNC_SetUniformValueFloat(*this, value);
}
//----------------------------------------------------------------------------
void ShaderUniform::SetValue(float value[2])
{
    GPU_DEVICE_FUNC_SetUniformValueFloat2(*this, value);
}
//----------------------------------------------------------------------------
void ShaderUniform::SetValue(const glm::mat4& value)
{
    GPU_DEVICE_FUNC_SetUniformValueMat4(*this, value);
}
//----------------------------------------------------------------------------
void ShaderUniform::SetValue(const glm::vec3& value)
{
    GPU_DEVICE_FUNC_SetUniformValueVec3(*this, value);
}
//----------------------------------------------------------------------------