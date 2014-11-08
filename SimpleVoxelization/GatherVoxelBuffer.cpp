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
    GLuint program = p->GetShaderProgram()->GetProgram();

    mSceneBBMinLoc = glGetUniformLocation(program, "SceneBBMin");
    mSceneBBExtensionLoc = glGetUniformLocation(program, "SceneBBExtension");
}
//----------------------------------------------------------------------------
void GatherVoxelBuffer::OnPreDispatch(unsigned int pass)
{
    vec3 min = SceneBB->Min;
    vec3 extension = SceneBB->GetExtension();
    glUniform3fv(mSceneBBMinLoc, 1, (GLfloat*)&min);
    glUniform3fv(mSceneBBExtensionLoc, 1, (GLfloat*)&extension);
}
//----------------------------------------------------------------------------
void GatherVoxelBuffer::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
//----------------------------------------------------------------------------