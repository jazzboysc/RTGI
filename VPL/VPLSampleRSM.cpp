#include "VPLSampleRSM.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLSampleRSM::VPLSampleRSM()
{
}
//----------------------------------------------------------------------------
VPLSampleRSM::~VPLSampleRSM()
{
    VPLSamplePattern = 0;
    VPLSampleTest = 0;
    RSMPosition = 0;
    RSMNormal = 0;
    RSMFlux = 0;
    VPLBuffer = 0;
}
//----------------------------------------------------------------------------
void VPLSampleRSM::OnGetShaderConstants()
{
    ComputePass* p = (ComputePass*)GetPass(0);
    ShaderProgram* program = p->GetShaderProgram();

    program->GetUniformLocation(&mRSMPositionLoc, "RSMPosition");
    program->GetUniformLocation(&mRSMNormalLoc, "RSMNormal");
    program->GetUniformLocation(&mRSMFluxLoc, "RSMFlux");
}
//----------------------------------------------------------------------------
void VPLSampleRSM::OnPreDispatch(unsigned int pass)
{
    VPLSamplePattern->BindToImageUnit(0, GL_READ_ONLY);
    VPLSampleTest->BindToImageUnit(1, GL_WRITE_ONLY);
    VPLBuffer->Bind(0);

    mRSMPositionLoc.SetValue(0);
    mRSMNormalLoc.SetValue(1);
    mRSMFluxLoc.SetValue(2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, RSMPosition->GetTexture());
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, RSMNormal->GetTexture());
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D_ARRAY, RSMFlux->GetTexture());
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP);
}
//----------------------------------------------------------------------------
void VPLSampleRSM::OnPostDispatch(unsigned int pass)
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