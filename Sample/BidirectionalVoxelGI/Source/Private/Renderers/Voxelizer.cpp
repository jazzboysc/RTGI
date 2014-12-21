#include "Voxelizer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ResetVoxelBuffer::ResetVoxelBuffer()
{
}
//----------------------------------------------------------------------------
ResetVoxelBuffer::~ResetVoxelBuffer()
{
}
//----------------------------------------------------------------------------
void ResetVoxelBuffer::OnGetShaderConstants()
{
}
//----------------------------------------------------------------------------
void ResetVoxelBuffer::OnPreDispatch(unsigned int pass)
{
}
//----------------------------------------------------------------------------
void ResetVoxelBuffer::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
//----------------------------------------------------------------------------

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
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
Voxelizer::Voxelizer(RenderSet* renderSet)
    :
    SubRenderer(renderSet)
{
}
//----------------------------------------------------------------------------
Voxelizer::~Voxelizer()
{
    delete mVoxelizationProjector;
    mResetVoxelBufferTask = 0;
    mGatherVoxelBufferTask = 0;
}
//----------------------------------------------------------------------------
void Voxelizer::Initialize(GPUDevice* device, int voxelGridDim, 
    int voxelGridLocalGroupDim, AABB* sceneBB)
{
    mVoxelGridDim = voxelGridDim;
    mVoxelGridLocalGroupDim = voxelGridLocalGroupDim;
    mGlobalDim = mVoxelGridDim / mVoxelGridLocalGroupDim;

    GLint globalX, globalY, globalZ;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &globalX);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &globalY);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &globalZ);
    assert(globalX >= mGlobalDim && globalY >= mGlobalDim &&
        globalZ >= mGlobalDim);

    // Create voxelization projector.
    mVoxelizationProjector = new Camera(false);
    mVoxelizationProjector->SetOrthogonalFrustum(10.5f, 1.0f, 0.01f, 20.5f);

    // Create reset voxel buffer task.
    ShaderProgramInfo resetVoxelBufferProgramInfo;
    resetVoxelBufferProgramInfo.CShaderFileName = 
        "BidirectionalVoxelGI/cResetVoxelBuffer.glsl";
    resetVoxelBufferProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passResetVoxelBuffer = new ComputePass(resetVoxelBufferProgramInfo);
    mResetVoxelBufferTask = new ResetVoxelBuffer();
    mResetVoxelBufferTask->AddPass(passResetVoxelBuffer);
    mResetVoxelBufferTask->CreateDeviceResource(device);

    // Create gather voxel buffer task.
    ShaderProgramInfo gatherVoxelBufferProgramInfo;
    gatherVoxelBufferProgramInfo.CShaderFileName = 
        "BidirectionalVoxelGI/cGatherVoxelBuffer.glsl";
    gatherVoxelBufferProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passGatherVoxelBuffer = new ComputePass(gatherVoxelBufferProgramInfo);
    mGatherVoxelBufferTask = new GatherVoxelBuffer();
    mGatherVoxelBufferTask->AddPass(passGatherVoxelBuffer);
    mGatherVoxelBufferTask->CreateDeviceResource(device);
    mGatherVoxelBufferTask->SceneBB = sceneBB;

    // Create scene voxel buffer.
    int voxelCount = mVoxelGridDim * mVoxelGridDim * mVoxelGridDim;
    int bufferSize = voxelCount * sizeof(unsigned int) * 4;
    AddGenericBufferTarget("VoxelBuffer", RDT_StructuredBuffer, bufferSize, 
        BU_Dynamic_Copy, BF_BindIndex, 0);

    // Create indirect command buffer.
    bufferSize = sizeof(unsigned int) * 5 + sizeof(float) * 35 
        + voxelCount*sizeof(float) * 4;
    AddGenericBufferTarget("IndirectCommandBuffer", RDT_StructuredBuffer, 
        bufferSize, BU_Dynamic_Copy, BF_BindIndex, 1);

    // Create gathered voxel GPU memory allocator counter.
    bufferSize = sizeof(unsigned int);
    AddGenericBufferTarget("GPUMemoryAllocatorCounter", 
        RDT_AtomicCounterBuffer, bufferSize, BU_Dynamic_Copy, BF_BindIndex, 0, 
        true, 0);
}
//----------------------------------------------------------------------------
void Voxelizer::Render(int technique, int pass)
{
    SubRenderer::Render(technique, pass, SRO_GenericBuffer, 0, 
        mVoxelizationProjector);
}
//----------------------------------------------------------------------------
void Voxelizer::OnRender(int technique, int pass, Camera* camera)
{
    // Reset voxel buffer pass.
    mResetVoxelBufferTask->Dispatch(0, mGlobalDim, mGlobalDim, mGlobalDim);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    assert(mRenderSet);
    int renderObjectCount = mRenderSet->GetRenderObjectCount();
    if( camera )
    {
        for( int i = 0; i < renderObjectCount; ++i )
        {
            RenderObject* renderObject = mRenderSet->GetRenderObject(i);
            renderObject->SetCamera(camera);
        }
    }
    glViewport(0, 0, mVoxelGridDim, mVoxelGridDim);
    for( int i = 0; i < renderObjectCount - 1; ++i )
    {
        RenderObject* renderObject = mRenderSet->GetRenderObject(i);
        renderObject->Render(technique, pass, this);
    }
    glViewport(0, 0, (mVoxelGridDim >> 4) + 4, (mVoxelGridDim >> 4) + 4);
    RenderObject* renderObject = mRenderSet->GetRenderObject(renderObjectCount - 1);
    renderObject->Render(technique, pass, this);

    // Gather voxel buffer pass.
    mGatherVoxelBufferTask->Dispatch(0, mGlobalDim, mGlobalDim, mGlobalDim);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}
//----------------------------------------------------------------------------
int Voxelizer::GetVoxelGridDim() const
{
    return mVoxelGridDim;
}
//----------------------------------------------------------------------------
int Voxelizer::GetVoxelGridLocalGroupDim() const
{
    return mVoxelGridLocalGroupDim;
}
//----------------------------------------------------------------------------