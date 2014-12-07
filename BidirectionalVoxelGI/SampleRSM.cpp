#include "SampleRSM.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SampleRSM::SampleRSM()
{
}
//----------------------------------------------------------------------------
SampleRSM::~SampleRSM()
{
    VPLSamplePattern = 0;
    VPLSampleTest = 0;
    VPLBuffer = 0;
}
//----------------------------------------------------------------------------
void SampleRSM::OnGetShaderConstants()
{
    ComputePass* p = (ComputePass*)GetPass(0);
    GLuint program = p->GetShaderProgram()->GetProgram();

    mRSMPositionLoc = glGetUniformLocation(program, "RSMPosition");
    mRSMNormalLoc = glGetUniformLocation(program, "RSMNormal");
    mRSMFluxLoc = glGetUniformLocation(program, "RSMFlux");
}
//----------------------------------------------------------------------------
void SampleRSM::OnPreDispatch(unsigned int pass)
{
    VPLSamplePattern->BindToImageUnit(0, GL_READ_ONLY);
    VPLSampleTest->BindToImageUnit(1, GL_WRITE_ONLY);

    glUniform1i(mRSMPositionLoc, 0);
    glUniform1i(mRSMNormalLoc, 1);
    glUniform1i(mRSMFluxLoc, 2);
}
//----------------------------------------------------------------------------
void SampleRSM::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
//----------------------------------------------------------------------------