#include "SVOVoxelizer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
BuildSVO::BuildSVO()
{
}
//----------------------------------------------------------------------------
BuildSVO::~BuildSVO()
{
}
//----------------------------------------------------------------------------
void BuildSVO::OnGetShaderConstants()
{
}
//----------------------------------------------------------------------------
void BuildSVO::OnPreDispatch(unsigned int)
{
}
//----------------------------------------------------------------------------
void BuildSVO::OnPostDispatch(unsigned int pass)
{
    switch( pass )
    {
    case BUILD_SVO_INIT_ROOT_PASS:
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        break;

    case BUILD_SVO_FLAG_NODES_PASS:
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        break;

    case BUILD_SVO_ALLOC_NODES_PASS:
        glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
        break;

    case BUILD_SVO_POST_ALLOC_NODES_PASS:
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);
        break;

    case BUILD_SVO_INIT_NODES_PASS:
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        break;

    default:
        break;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
GatherVoxelFragmentListInfo::GatherVoxelFragmentListInfo()
{
}
//----------------------------------------------------------------------------
GatherVoxelFragmentListInfo::~GatherVoxelFragmentListInfo()
{
}
//----------------------------------------------------------------------------
void GatherVoxelFragmentListInfo::OnGetShaderConstants()
{
}
//----------------------------------------------------------------------------
void GatherVoxelFragmentListInfo::OnPreDispatch(unsigned int)
{
    glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
}
//----------------------------------------------------------------------------
void GatherVoxelFragmentListInfo::OnPostDispatch(unsigned int)
{
    glMemoryBarrier(GL_COMMAND_BARRIER_BIT);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
SVOVoxelizer::SVOVoxelizer(GPUDevice* device, RenderSet* renderSet)
    :
    Voxelizer(device, renderSet)
{
    mVoxelizerType = VT_SVO;
    mSceneBBMaxLength = 0.0f;
}
//----------------------------------------------------------------------------
SVOVoxelizer::~SVOVoxelizer()
{
    mAtomicCounterBuffer = 0;
    mVoxelFragmentListBuffer = 0;
    mSVOBuffer = 0;
    mSVOUniformBuffer = 0;

    mGatherVoxelFragmentListInfoTask = 0;
    mBuildSVOTask = 0;
}
//----------------------------------------------------------------------------
void SVOVoxelizer::Initialize(GPUDevice* device, int voxelGridDim, 
    AABB* sceneBB)
{
    VoxelGridDim = voxelGridDim;
    mSceneBB = sceneBB;

    // Create gather voxel fragment list info task.
    ShaderProgramInfo gatherVoxelFragmentListInfoProgramInfo;
    gatherVoxelFragmentListInfoProgramInfo.CShaderFileName = 
        "BidirectionalVoxelGI/cGatherVoxelFragmentListInfo.glsl";
    gatherVoxelFragmentListInfoProgramInfo.ShaderStageFlag = 
        ShaderType::ST_Compute;

    ComputePass* passGatherVoxelFragmentListInfo = new ComputePass(
        gatherVoxelFragmentListInfoProgramInfo);
    mGatherVoxelFragmentListInfoTask = new GatherVoxelFragmentListInfo();
    mGatherVoxelFragmentListInfoTask->AddPass(passGatherVoxelFragmentListInfo);
    mGatherVoxelFragmentListInfoTask->CreateDeviceResource(mDevice);

    // Create build SVO task.
    ShaderProgramInfo buildSVOInitRootProgramInfo;
    buildSVOInitRootProgramInfo.VShaderFileName = 
        "BidirectionalVoxelGI/vBuildSVOInitRoot.glsl";
    buildSVOInitRootProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex;
    ComputePass* passBuildSVOInitRoot = new ComputePass(
        buildSVOInitRootProgramInfo);

    ShaderProgramInfo buildSVOFlagNodesProgramInfo;
    buildSVOFlagNodesProgramInfo.VShaderFileName = 
        "BidirectionalVoxelGI/vBuildSVOFlagNodes.glsl";
    buildSVOFlagNodesProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex;
    ComputePass* passBuildSVOFlagNodes = new ComputePass(
        buildSVOFlagNodesProgramInfo);

    ShaderProgramInfo buildSVOAllocateNodesProgramInfo;
    buildSVOAllocateNodesProgramInfo.VShaderFileName = 
        "BidirectionalVoxelGI/vBuildSVOAllocateNodes.glsl";
    buildSVOAllocateNodesProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex;
    ComputePass* passBuildSVOAllocateNodes = new ComputePass(
        buildSVOAllocateNodesProgramInfo);

    ShaderProgramInfo buildSVOPostAllocateNodesProgramInfo;
    buildSVOPostAllocateNodesProgramInfo.VShaderFileName = 
        "BidirectionalVoxelGI/vBuildSVOPostAllocateNodes.glsl";
    buildSVOPostAllocateNodesProgramInfo.ShaderStageFlag = 
        ShaderType::ST_Vertex;
    ComputePass* passBuildSVOPostAllocateNodes = new ComputePass(
        buildSVOPostAllocateNodesProgramInfo);

    ShaderProgramInfo buildSVOInitNodesProgramInfo;
    buildSVOInitNodesProgramInfo.VShaderFileName = 
        "BidirectionalVoxelGI/vBuildSVOInitNodes.glsl";
    buildSVOInitNodesProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex;
    ComputePass* passBuildSVOInitNodes = new ComputePass(
        buildSVOInitNodesProgramInfo);

    ShaderProgramInfo buildSVOSplatLeafNodesProgramInfo;
    buildSVOSplatLeafNodesProgramInfo.VShaderFileName = 
        "BidirectionalVoxelGI/vBuildSVOSplatLeafNodes.glsl";
    buildSVOSplatLeafNodesProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex;
    ComputePass* passBuildSVOSplatLeafNodes = new ComputePass(
        buildSVOSplatLeafNodesProgramInfo);

    mBuildSVOTask = new BuildSVO();
    mBuildSVOTask->AddPass(passBuildSVOInitRoot);
    mBuildSVOTask->AddPass(passBuildSVOFlagNodes);
    mBuildSVOTask->AddPass(passBuildSVOAllocateNodes);
    mBuildSVOTask->AddPass(passBuildSVOPostAllocateNodes);
    mBuildSVOTask->AddPass(passBuildSVOInitNodes);
    mBuildSVOTask->AddPass(passBuildSVOSplatLeafNodes);
    mBuildSVOTask->CreateDeviceResource(mDevice);

    // Create voxel fragment list buffer.
    unsigned int voxelCount = VoxelGridDim * VoxelGridDim * VoxelGridDim;
    mVoxelFragmentListBuffer = new StructuredBuffer();
    unsigned int voxelFragmentCount = 
        unsigned int((float)voxelCount*0.2f); // voxel fragment ratio.
    size_t bufferSize = sizeof(VoxelFragmentBufferHead) + 
        voxelFragmentCount*sizeof(VoxelFragment);
    mVoxelFragmentListBuffer->ReserveMutableDeviceResource(mDevice, 
        bufferSize, BU_Dynamic_Copy);

    // Create SVO buffer.
    mSVOMaxLevel = (unsigned int)glm::log2((float)VoxelGridDim);
    mSVOBuffer = new StructuredBuffer();
    // interior node ratio (1/7, fixed) and leaf node ratio.
    mSVONodeMaxCount = unsigned int((float)voxelCount*(0.143f + 0.05f));
    bufferSize = sizeof(SVONodeBufferHead) + mSVONodeMaxCount*sizeof(SVONode);
    mSVOBuffer->ReserveMutableDeviceResource(mDevice, bufferSize, 
        BU_Dynamic_Copy);

    // Create SVO uniform buffer.
    mSVOUniformBuffer = new UniformBuffer();
    bufferSize = sizeof(unsigned int) * 2;
    mSVOUniformBuffer->ReserveMutableDeviceResource(mDevice, bufferSize, 
        BU_Dynamic_Draw);
    unsigned int svoUniformBufferData[2] = { 0, VoxelGridDim };
    mSVOUniformBuffer->UpdateSubData(0, 0, sizeof(unsigned int) * 2, 
        (void*)svoUniformBufferData);

    // Create atomic counter buffer. We create 2 atomic counters here.
    mAtomicCounterBuffer = new AtomicCounterBuffer();
#ifdef DEBUG_VOXEL
    mAtomicCounterBuffer->ReserveMutableDeviceResource(mDevice, 
        sizeof(unsigned int) * 2, BU_Dynamic_Copy);
#else
    mAtomicCounterBuffer->ReserveImmutableDeviceResource(mDevice, 
        sizeof(unsigned int) * 2);
#endif

    // Initialize scene bounding box buffer.
    vec3 sceneBBMin = mSceneBB->Min;
    vec3 center = mSceneBB->GetBoxCenter();
    vec3 extension = mSceneBB->GetExtension();
    vec3 inv2extension = vec3(1.0f / (2.0f*extension.x), 
        1.0f / (2.0f*extension.y), 1.0f / (2.0f*extension.z));
    mVoxelFragmentListBuffer->Bind();
    VoxelFragmentBufferHead* bufferData = 
        (VoxelFragmentBufferHead*)mVoxelFragmentListBuffer->Map(BA_Write_Only);
    bufferData->SceneBBMin = vec4(sceneBBMin, 1.0);
    bufferData->SceneBBCenter = vec4(center, 1.0);
    bufferData->SceneBBExtension = vec4(extension, 0.0);
    bufferData->Inv2SceneBBExtension = vec4(inv2extension, 0.0);
    mVoxelFragmentListBuffer->Unmap();
}
//----------------------------------------------------------------------------
void SVOVoxelizer::Render(int technique, int pass)
{
    SubRenderer::Render(technique, pass, SRO_GenericBuffer, 0);
}
//----------------------------------------------------------------------------
static GLint oldViewport[4];
//----------------------------------------------------------------------------
void SVOVoxelizer::OnRender(int technique, int pass, Camera*)
{
    // Cache old viewport values and set new values.
    glGetIntegerv(GL_VIEWPORT, oldViewport);

    // Reset counters.
    mAtomicCounterBuffer->Bind(0);
#ifdef DEBUG_VOXEL
    unsigned int* counterData = 
        (unsigned int*)mAtomicCounterBuffer->Map(BA_Write_Only);
    assert(counterData);
    counterData[0] = 0;
    counterData[1] = 0;
    mAtomicCounterBuffer->Unmap();
#else
    unsigned int counterData[2];
    counterData[0] = 0;
    counterData[1] = 0;
    mAtomicCounterBuffer->Clear(BIF_R32UI, BF_R32UI, BCT_Unsigned_Int, 
        counterData);
#endif

    //----------------------- Begin building SVO -----------------------//

    // Scene voxelization pass.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    mVoxelFragmentListBuffer->Bind(1);
    VoxelizeScene(technique, pass);

    // Gather voxel fragment list info pass.
    mVoxelFragmentListBuffer->Bind(1);
    mGatherVoxelFragmentListInfoTask->DispatchCompute(0, 1, 1, 1);
#ifdef DEBUG_VOXEL
    mVoxelFragmentListBuffer->Bind();
    unsigned int* dispatchIndirectCommandbufferData = 
        (unsigned int*)mVoxelFragmentListBuffer->Map(BA_Read_Only);
    //infoPanel->SetTimingLabelValue("GVF Count", 
    (double)dispatchIndirectCommandbufferData[0]);
    mVoxelFragmentListBuffer->Unmap();
#endif

    // Build SVO init root pass.
    mSVOUniformBuffer->Bind(0);
    mVoxelFragmentListBuffer->Bind(1);
    mSVOBuffer->Bind(3);
    mBuildSVOTask->DispatchVertex(BUILD_SVO_INIT_ROOT_PASS, 1);
#ifdef DEBUG_VOXEL
    mSVOBuffer->Bind();
    void* svoBufferData = mSVOBuffer->Map(BA_Read_Only);
    SVONodeBufferHead* svoBufferHeadData = (SVONodeBufferHead*)svoBufferData;
    SVONodeTile* svoNodeTileData = (SVONodeTile*)(svoBufferHeadData + 1);
    mSVOBuffer->Unmap();
#endif

    unsigned int curLevel = 1;
    for( ; curLevel < mSVOMaxLevel; ++curLevel )
    {
        // Update SVO uniform buffer.
        mSVOUniformBuffer->UpdateSubData(0, 0, sizeof(unsigned int), 
            (void*)&curLevel);
#ifdef DEBUG_VOXEL
        unsigned int* svoUniformBufferData = 
            (unsigned int*)mSVOUniformBuffer->Map(BA_Read_Only);
        mSVOUniformBuffer->Unmap();
#endif

        // Flag SVO nodes pass.
        mVoxelFragmentListBuffer->Bind(1);
        mSVOBuffer->Bind(3);
        mBuildSVOTask->DispatchVertexIndirect(BUILD_SVO_FLAG_NODES_PASS,
            mVoxelFragmentListBuffer, 0);
#ifdef DEBUG_VOXEL
        mSVOBuffer->Bind();
        svoBufferData = mSVOBuffer->Map(BA_Read_Only);
        svoBufferHeadData = (SVONodeBufferHead*)svoBufferData;
        svoNodeTileData = (SVONodeTile*)(svoBufferHeadData + 1);
        mSVOBuffer->Unmap();
#endif

        // Allocate SVO nodes pass.
        mBuildSVOTask->DispatchVertexIndirect(BUILD_SVO_ALLOC_NODES_PASS,
            mSVOBuffer, 0);
#ifdef DEBUG_VOXEL
        mSVOBuffer->Bind();
        svoBufferData = mSVOBuffer->Map(BA_Read_Only);
        svoBufferHeadData = (SVONodeBufferHead*)svoBufferData;
        svoNodeTileData = (SVONodeTile*)(svoBufferHeadData + 1);
        mSVOBuffer->Unmap();
#endif

        // Post allocate SVO nodes pass.
        mBuildSVOTask->DispatchVertex(BUILD_SVO_POST_ALLOC_NODES_PASS, 1);

        // Init SVO nodes pass.
        mBuildSVOTask->DispatchVertexIndirect(BUILD_SVO_INIT_NODES_PASS,
            mSVOBuffer, 0);
#ifdef DEBUG_VOXEL
        mSVOBuffer->Bind();
        svoBufferData = mSVOBuffer->Map(BA_Read_Only);
        svoBufferHeadData = (SVONodeBufferHead*)svoBufferData;
        svoNodeTileData = (SVONodeTile*)(svoBufferHeadData + 1);
        mSVOBuffer->Unmap();
#endif
    }

    // Splat SVO leaf nodes pass.
    mVoxelFragmentListBuffer->Bind(1);
    mSVOBuffer->Bind(3);
    mBuildSVOTask->DispatchVertexIndirect(BUILD_SVO_SPLAT_LEAF_NODES_PASS,
        mVoxelFragmentListBuffer, 0);
#ifdef DEBUG_VOXEL
    mSVOBuffer->Bind();
    svoBufferData = mSVOBuffer->Map(BA_Read_Only);
    svoBufferHeadData = (SVONodeBufferHead*)svoBufferData;
    svoNodeTileData = (SVONodeTile*)(svoBufferHeadData + 1);
    mSVOBuffer->Unmap();
#endif

    //------------------------ End building SVO ------------------------//

    // Restore device states.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glViewport(oldViewport[0], oldViewport[1], oldViewport[2],
        oldViewport[3]);
}
//----------------------------------------------------------------------------
void SVOVoxelizer::VoxelizeScene(int technique, int pass)
{
    vec3 sceneBBLen = mSceneBB->GetExtension() * 2.0f;
    mSceneBBMaxLength = RTGI_MAX(sceneBBLen.x,
        RTGI_MAX(sceneBBLen.y, sceneBBLen.z));

    assert(mRenderSet);
    int renderObjectCount = mRenderSet->GetRenderObjectCount();
    for( int i = 0; i < renderObjectCount; ++i )
    {
        // TODO:
        // Only support trangle mesh for now.
        TriangleMesh* mesh = (TriangleMesh*)mRenderSet->GetRenderObject(i);
        float triangleDim = mesh->GetTriangleMaxEdgeLength();
        float ratio = triangleDim / mSceneBBMaxLength;
        int rasterizerDim = (int)ceilf(ratio * (float)VoxelGridDim) +
            RasterizerDimBias;

        glViewport(0, 0, rasterizerDim, rasterizerDim);
        mesh->Render(technique, pass, this);
    }
}
//----------------------------------------------------------------------------