#include "VPLGenerator.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLGenerator::VPLGenerator(RenderSet* renderSet)
    :
    SubRenderer(renderSet)
{
}
//----------------------------------------------------------------------------
VPLGenerator::~VPLGenerator()
{
}
//----------------------------------------------------------------------------
void VPLGenerator::SetRSMRenderer(RSMRenderer* rsm)
{
    mRSM = rsm;

    RendererInputDataView view;
    view.Type = RDT_Texture;
    view.Sampler.MinFilter = FT_Nearest;
    view.Sampler.MagFilter = FT_Nearest;
    view.Sampler.WrapS = WT_Clamp;
    view.Sampler.WrapT = WT_Clamp;

    ClearInputDependency();

    view.BindingSlot = 0;
    AddInputDependency(rsm, "RSMPosition", &view);

    view.BindingSlot = 1;
    AddInputDependency(rsm, "RSMNormal", &view);

    view.BindingSlot = 2;
    AddInputDependency(rsm, "RSMFlux", &view);
}
//----------------------------------------------------------------------------
RSMRenderer* VPLGenerator::GetRSMRenderer() const
{
    return mRSM;
}
//----------------------------------------------------------------------------
void VPLGenerator::Run()
{
    SubRenderer::Render(0, 0, SRO_GenericBuffer, 0);
}
//----------------------------------------------------------------------------
void VPLGenerator::OnRender(int, int, Camera*)
{
    mSampleRSMTask->Dispatch(0, VPL_SAMPLE_COUNT, 1, 1);
}
//----------------------------------------------------------------------------
void VPLGenerator::Initialize()
{
    // Create VPL sample pattern.
    mVPLSamplePattern = new Texture1D();
    mVPLSamplePattern->CreateUniformRandomTexture(VPL_SAMPLE_COUNT, 4);
    mVPLSampleTest = new Texture1D();
    mVPLSampleTest->LoadFromSystemMemory(GL_RGBA32F, VPL_SAMPLE_COUNT, GL_RGBA, GL_FLOAT, 0);

    // Create VPL buffer.
    GLuint vplBufferSize = (sizeof(vec4) * 3 + sizeof(mat4)) * VPL_SAMPLE_COUNT;
    mVPLBuffer = new StructuredBuffer();
    mVPLBuffer->ReserveDeviceResource(vplBufferSize, BU_Dynamic_Copy);

    // Create VPL sample compute tasks.
    ShaderProgramInfo sampleRSMProgramInfo;
    sampleRSMProgramInfo.CShaderFileName = "cSampleRSM.glsl";
    sampleRSMProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passSampleRSM = new ComputePass(sampleRSMProgramInfo);
    mSampleRSMTask = new SampleRSM();
    mSampleRSMTask->AddPass(passSampleRSM);
    mSampleRSMTask->CreateDeviceResource();
    mSampleRSMTask->VPLSamplePattern = mVPLSamplePattern;
    mSampleRSMTask->VPLSampleTest = mVPLSampleTest;
    mSampleRSMTask->VPLBuffer = mVPLBuffer;
}
//----------------------------------------------------------------------------