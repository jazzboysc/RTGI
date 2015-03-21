#include "GridVoxelizer.h"

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
GridVoxelizer::GridVoxelizer(GPUDevice* device, RenderSet* renderSet)
    :
    Voxelizer(device, renderSet)
{
    mVoxelizerType = VT_Grid;
    mSceneBBMaxLength = 0.0f;
}
//----------------------------------------------------------------------------
GridVoxelizer::~GridVoxelizer()
{
    mResetVoxelBufferTask = 0;
}
//----------------------------------------------------------------------------
void GridVoxelizer::Initialize(GPUDevice* device, int voxelGridDim, 
    int voxelGridLocalGroupDim, AABB* sceneBB)
{
    VoxelGridDim = voxelGridDim;
    mVoxelGridLocalGroupDim = voxelGridLocalGroupDim;
    mGlobalDim = VoxelGridDim / mVoxelGridLocalGroupDim;
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
        "VPLviaSVOGI/cResetVoxelBuffer.glsl";
    resetVoxelBufferProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passResetVoxelBuffer = new ComputePass(resetVoxelBufferProgramInfo);
    mResetVoxelBufferTask = new ResetVoxelBuffer();
    mResetVoxelBufferTask->AddPass(passResetVoxelBuffer);
    mResetVoxelBufferTask->CreateDeviceResource(device);

    // Create scene voxel buffer.
    int voxelCount = VoxelGridDim * VoxelGridDim * VoxelGridDim;
    int bufferSize = voxelCount * sizeof(unsigned int) * 4;
    AddGenericBufferTarget(RTGI_GridVoxelizer_VoxelBuffer_Name, 
        RDT_StructuredBuffer, bufferSize, BU_Dynamic_Copy, BF_BindIndex, 0);
}
//----------------------------------------------------------------------------
static GLint oldViewport[4];
//----------------------------------------------------------------------------
void GridVoxelizer::OnRender(int technique, int pass, Camera*)
{
    // Cache old viewport values and set new values.
    glGetIntegerv(GL_VIEWPORT, oldViewport);

    // Reset voxel buffer pass.
    mResetVoxelBufferTask->DispatchCompute(0, mGlobalDim, mGlobalDim, 
        mGlobalDim);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    VoxelizeScene(technique, pass);

    // Restore device states.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glViewport(oldViewport[0], oldViewport[1], oldViewport[2],
        oldViewport[3]);
}
//----------------------------------------------------------------------------