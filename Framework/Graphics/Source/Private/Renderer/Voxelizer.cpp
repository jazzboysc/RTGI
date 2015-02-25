//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Voxelizer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Voxelizer::Voxelizer(GPUDevice* device, RenderSet* renderSet)
    :
    SubRenderer(device, renderSet),
    mVoxelizerType(VT_Unknown)
{
    RasterizerDimBias = 0;
    VoxelGridDim = 0;
}
//----------------------------------------------------------------------------
Voxelizer::~Voxelizer()
{
}
//----------------------------------------------------------------------------
void Voxelizer::Render(int technique, int pass)
{
    SubRenderer::Render(technique, pass, SRO_GenericBuffer, 0);
}
//----------------------------------------------------------------------------
void Voxelizer::VoxelizeScene(int technique, int pass)
{
    glm::vec3 sceneBBLen = mSceneBB->GetExtension() * 2.0f;
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