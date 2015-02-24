#include "SVOVoxelizer.h"

using namespace RTGI;

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
}
//----------------------------------------------------------------------------
void SVOVoxelizer::Initialize(GPUDevice* device, int voxelGridDim,
    int voxelGridLocalGroupDim, AABB* sceneBB)
{

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

    vec3 sceneBBLen = mSceneBB->GetExtension() * 2.0f;
    mSceneBBMaxLength = RTGI_MAX(sceneBBLen.x, 
        RTGI_MAX(sceneBBLen.y, sceneBBLen.z));

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
        int rasterizerDim = (int)ceilf(ratio * (float)VoxelGridDim) + 
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