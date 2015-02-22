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
	this->mUniformHandle->Device->SetUniformValueInt(this, value);
}
//----------------------------------------------------------------------------
void ShaderUniform::SetValue(float value)
{
	this->mUniformHandle->Device->SetUniformValueFloat(this, value);
}
//----------------------------------------------------------------------------
void ShaderUniform::SetValue(float value[2])
{
	this->mUniformHandle->Device->SetUniformValueFloat2(this, value);
}
//----------------------------------------------------------------------------
void ShaderUniform::SetValue(const glm::mat4& value)
{
	this->mUniformHandle->Device->SetUniformValueMat4(this, &value);
}
//----------------------------------------------------------------------------
void ShaderUniform::SetValue(const glm::vec3& value)
{
	this->mUniformHandle->Device->SetUniformValueVec3(this, &value);
}
//----------------------------------------------------------------------------