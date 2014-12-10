#include "VPLGenerator.h"

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
}
//----------------------------------------------------------------------------
void SampleRSM::OnGetShaderConstants()
{
    ComputePass* p = (ComputePass*)GetPass(0);
    ShaderProgram* program = p->GetShaderProgram();

    GPU_DEVICE_FUNC_GetUniformLocation(program, mRSMPositionLoc, "RSMPosition");
    GPU_DEVICE_FUNC_GetUniformLocation(program, mRSMNormalLoc, "RSMNormal");
    GPU_DEVICE_FUNC_GetUniformLocation(program, mRSMFluxLoc, "RSMFlux");
}
//----------------------------------------------------------------------------
void SampleRSM::OnPreDispatch(unsigned int pass)
{
    VPLSamplePattern->BindToImageUnit(0, GL_READ_ONLY);
    VPLSampleTest->BindToImageUnit(1, GL_WRITE_ONLY);

    GPU_DEVICE_FUNC_SetUniformValueInt(mRSMPositionLoc, 0);
    GPU_DEVICE_FUNC_SetUniformValueInt(mRSMNormalLoc, 1);
    GPU_DEVICE_FUNC_SetUniformValueInt(mRSMFluxLoc, 2);
}
//----------------------------------------------------------------------------
void SampleRSM::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
//----------------------------------------------------------------------------

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
    view.BindingType = BF_BindIndex;
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
    mSampleRSMTask->Dispatch(0, mVPLCount, 1, 1);
}
//----------------------------------------------------------------------------
void VPLGenerator::Initialize(GPUDevice* device, int vplCount)
{
    mVPLCount = vplCount;

    // Create VPL sample pattern.
    mVPLSamplePattern = new Texture1D();
    mVPLSamplePattern->CreateUniformRandomTexture(mVPLCount, 4);
    mVPLSampleTest = new Texture1D();
    mVPLSampleTest->LoadFromSystemMemory(GL_RGBA32F, mVPLCount, GL_RGBA,
        GL_FLOAT, 0);

    // Create VPL buffer.
    GLuint vplBufferSize = (sizeof(vec4) * 3 + sizeof(mat4)) * mVPLCount;
    AddGenericBufferTarget("VPLBuffer", RDT_StructuredBuffer, vplBufferSize, 
        BU_Dynamic_Copy, BF_BindIndex, 0);

    // Create VPL sample compute tasks.
    ShaderProgramInfo sampleRSMProgramInfo;
    sampleRSMProgramInfo.CShaderFileName = "cSampleRSM.glsl";
    sampleRSMProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passSampleRSM = new ComputePass(sampleRSMProgramInfo);
    mSampleRSMTask = new SampleRSM();
    mSampleRSMTask->AddPass(passSampleRSM);
    mSampleRSMTask->CreateDeviceResource(device);
    mSampleRSMTask->VPLSamplePattern = mVPLSamplePattern;
    mSampleRSMTask->VPLSampleTest = mVPLSampleTest;
}
//----------------------------------------------------------------------------