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

    program->GetUniformLocation(&mRSMPositionLoc, "RSMPosition");
    program->GetUniformLocation(&mRSMNormalLoc, "RSMNormal");
    program->GetUniformLocation(&mRSMFluxLoc, "RSMFlux");
}
//----------------------------------------------------------------------------
void SampleRSM::OnPreDispatch(unsigned int pass)
{
    VPLSamplePattern->BindToImageUnit(0, BA_Read_Only);
    VPLSampleTest->BindToImageUnit(1, BA_Write_Only);

    mRSMPositionLoc.SetValue(0);
    mRSMNormalLoc.SetValue(1);
    mRSMFluxLoc.SetValue(2);
}
//----------------------------------------------------------------------------
void SampleRSM::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
VPLGenerator::VPLGenerator(GPUDevice* device, RenderSet* renderSet)
    :
    SubRenderer(device, renderSet)
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
    mVPLSamplePattern->CreateUniformRandomTexture(device, mVPLCount, 4);
    mVPLSampleTest = new Texture1D();
    mVPLSampleTest->LoadFromSystemMemory(device, TIF_RGBA32F, mVPLCount, 
        TF_RGBA, TCT_Float, 0);

    // Create VPL buffer.
    GLuint vplBufferSize = (sizeof(vec4) * 3 + sizeof(mat4)) * mVPLCount;
    AddGenericBufferTarget(RTGI_VPLGenerator_VPLBuffer_Name, 
        RDT_StructuredBuffer, vplBufferSize, BU_Dynamic_Copy, BF_BindIndex, 0);

    // Create VPL sample compute tasks.
    ShaderProgramInfo sampleRSMProgramInfo;
    sampleRSMProgramInfo.CShaderFileName = "BidirectionalVoxelGI/cSampleRSM.glsl";
    sampleRSMProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passSampleRSM = new ComputePass(sampleRSMProgramInfo);
    mSampleRSMTask = new SampleRSM();
    mSampleRSMTask->AddPass(passSampleRSM);
    mSampleRSMTask->CreateDeviceResource(device);
    mSampleRSMTask->VPLSamplePattern = mVPLSamplePattern;
    mSampleRSMTask->VPLSampleTest = mVPLSampleTest;
}
//----------------------------------------------------------------------------