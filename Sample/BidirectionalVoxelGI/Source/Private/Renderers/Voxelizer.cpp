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
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
Voxelizer::Voxelizer(GPUDevice* device, RenderSet* renderSet)
    :
    SubRenderer(device, renderSet)
{
    mSceneBBMaxLength = 0.0f;
    RasterizerDimBias = 0;
}
//----------------------------------------------------------------------------
Voxelizer::~Voxelizer()
{
    mResetVoxelBufferTask = 0;
}
//----------------------------------------------------------------------------
void Voxelizer::Initialize(GPUDevice* device, int voxelGridDim, 
    int voxelGridLocalGroupDim, AABB* sceneBB)
{
    mVoxelGridDim = voxelGridDim;
    mVoxelGridLocalGroupDim = voxelGridLocalGroupDim;
    mGlobalDim = mVoxelGridDim / mVoxelGridLocalGroupDim;
    mSceneBB = sceneBB;

    GLint globalX, globalY, globalZ;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &globalX);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &globalY);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &globalZ);
    assert(globalX >= mGlobalDim && globalY >= mGlobalDim &&
        globalZ >= mGlobalDim);

    // Create reset voxel buffer task.
    ShaderProgramInfo resetVoxelBufferProgramInfo;
    resetVoxelBufferProgramInfo.CShaderFileName = 
        "BidirectionalVoxelGI/cResetVoxelBuffer.glsl";
    resetVoxelBufferProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passResetVoxelBuffer = new ComputePass(resetVoxelBufferProgramInfo);
    mResetVoxelBufferTask = new ResetVoxelBuffer();
    mResetVoxelBufferTask->AddPass(passResetVoxelBuffer);
    mResetVoxelBufferTask->CreateDeviceResource(device);

    // Create scene voxel buffer.
    int voxelCount = mVoxelGridDim * mVoxelGridDim * mVoxelGridDim;
    int bufferSize = voxelCount * sizeof(unsigned int) * 4;
    AddGenericBufferTarget(RTGI_Voxelizer_VoxelBuffer_Name, RDT_StructuredBuffer,
        bufferSize, BU_Dynamic_Copy, BF_BindIndex, 0);
}
//----------------------------------------------------------------------------
void Voxelizer::Render(int technique, int pass)
{
    SubRenderer::Render(technique, pass, SRO_GenericBuffer, 0);
}
//----------------------------------------------------------------------------
static GLint oldViewport[4];
//----------------------------------------------------------------------------
void Voxelizer::OnRender(int technique, int pass, Camera*)
{
    // Cache old viewport values and set new values.
    glGetIntegerv(GL_VIEWPORT, oldViewport);

    vec3 sceneBBLen = mSceneBB->GetExtension() * 2.0f;
    mSceneBBMaxLength = RTGI_MAX(sceneBBLen.x, 
        RTGI_MAX(sceneBBLen.y, sceneBBLen.z));

    // Reset voxel buffer pass.
    mResetVoxelBufferTask->Dispatch(0, mGlobalDim, mGlobalDim, mGlobalDim);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    assert(mRenderSet);
    int renderObjectCount = mRenderSet->GetRenderObjectCount();
    for( int i = 0; i < renderObjectCount; ++i )
    {
        // TODO:
        // Only support trangle mesh for now.
        TriangleMesh* mesh = (TriangleMesh*)mRenderSet->GetRenderObject(i);
        float triangleDim = mesh->GetTriangleMaxEdgeLength();
        float ratio = triangleDim / mSceneBBMaxLength;
        int rasterizerDim = (int)ceilf(ratio * (float)mVoxelGridDim) + 
            RasterizerDimBias;

        glViewport(0, 0, rasterizerDim, rasterizerDim);
        mesh->Render(technique, pass, this);
    }

    // Restore device states.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glViewport(oldViewport[0], oldViewport[1], oldViewport[2],
        oldViewport[3]);
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