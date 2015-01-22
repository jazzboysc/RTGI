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
    VPLSamplePattern->BindToImageUnit(0, BA_Read_Only);
    VPLSampleTest->BindToImageUnit(1, BA_Write_Only);
    VPLBuffer->Bind(0);

    mRSMPositionLoc.SetValue(0);
    mRSMNormalLoc.SetValue(1);
    mRSMFluxLoc.SetValue(2);

    SamplerDesc sampler;
    sampler.MinFilter = FT_Nearest;
    sampler.MagFilter = FT_Nearest;
    sampler.WrapS = WT_Clamp;
    sampler.WrapT = WT_Clamp;

    RSMPosition->BindToSampler(0, &sampler);
    RSMNormal->BindToSampler(1, &sampler);
    RSMFlux->BindToSampler(2, &sampler);
}
//----------------------------------------------------------------------------
void VPLSampleRSM::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

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