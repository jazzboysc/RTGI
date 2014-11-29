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
    RSMPosition = 0;
    RSMNormal = 0;
    RSMFlux = 0;
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
    VPLBuffer->Bind(0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, RSMPosition->GetTexture());
    glUniform1i(mRSMPositionLoc, 0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, RSMNormal->GetTexture());
    glUniform1i(mRSMNormalLoc, 1);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D_ARRAY, RSMFlux->GetTexture());
    glUniform1i(mRSMFluxLoc, 2);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP);
}
//----------------------------------------------------------------------------
void SampleRSM::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // Test.
    //float* srcData = new float[16 * 4];
    //float* dstData = new float[16 * 4];

    //VPLSamplePattern->GetDataFromGPUMemory(srcData);
    //VPLSampleTest->GetDataFromGPUMemory(dstData);

    //int res = memcmp(srcData, dstData, 256);
    //vec4* vplBufferData = (vec4*)VPLBuffer->Map(GL_READ_ONLY);
    //VPLBuffer->Unmap();

    //delete[] srcData;
    //delete[] dstData;
}
//----------------------------------------------------------------------------