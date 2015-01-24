#include "GatherVoxelBuffer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
GatherVoxelBuffer::GatherVoxelBuffer()
{
}
//----------------------------------------------------------------------------
GatherVoxelBuffer::~GatherVoxelBuffer()
{
}
//----------------------------------------------------------------------------
void GatherVoxelBuffer::OnGetShaderConstants()
{
    ComputePass* p = (ComputePass*)GetPass(0);
    ShaderProgram* program = p->GetShaderProgram();

    program->GetUniformLocation(&mSceneBBMinLoc, "SceneBBMin");
    program->GetUniformLocation(&mSceneBBExtensionLoc, "SceneBBExtension");
}
//----------------------------------------------------------------------------
void GatherVoxelBuffer::OnPreDispatch(unsigned int pass)
{
    vec3 min = SceneBB->Min;
    vec3 extension = SceneBB->GetExtension();
    mSceneBBMinLoc.SetValue(min);
    mSceneBBExtensionLoc.SetValue(extension);
}
//----------------------------------------------------------------------------
void GatherVoxelBuffer::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_COMMAND_BARRIER_BIT);
}
//----------------------------------------------------------------------------