//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "RenderSequence.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RenderSequence::RenderSequence(Material* material, Camera* camera)
    :
    RenderObject(material)
{
    SetCamera(camera);
    mActiveObjectIndex = 0;
}
//----------------------------------------------------------------------------
RenderSequence::~RenderSequence()
{
    for( int i = 0; i < (int)mObjects.size(); ++i )
    {
        mObjects[i] = 0;
    }
}
//----------------------------------------------------------------------------
int RenderSequence::GetVoxelizerRasterDimension(Voxelizer* voxelizer)
{
    return mObjects[mActiveObjectIndex]->GetVoxelizerRasterDimension(
        voxelizer);
}
//----------------------------------------------------------------------------
Primitive* RenderSequence::GetPrimitive() const
{
    return mObjects[mActiveObjectIndex]->GetPrimitive();
}
//----------------------------------------------------------------------------
Material* RenderSequence::GetMaterial() const
{
    return mObjects[mActiveObjectIndex]->GetMaterial();
}
//----------------------------------------------------------------------------
void RenderSequence::SetRenderCache(RenderCache* renderCache)
{
    mObjects[mActiveObjectIndex]->SetRenderCache(renderCache);
}
//----------------------------------------------------------------------------
RenderCache* RenderSequence::GetRenderCache()
{
    return mObjects[mActiveObjectIndex]->GetRenderCache();
}
//----------------------------------------------------------------------------
void RenderSequence::UpdateRenderCache()
{
    mObjects[mActiveObjectIndex]->UpdateRenderCache();
}
//----------------------------------------------------------------------------
void RenderSequence::Render(int technique, int pass, SubRenderer* subRenderer)
{
    mObjects[mActiveObjectIndex]->Render(technique, pass, subRenderer);
}
//----------------------------------------------------------------------------
void RenderSequence::OnUpdateShaderConstants(int technique, int pass)
{
    mObjects[mActiveObjectIndex]->OnUpdateShaderConstants(technique, pass);
}
//----------------------------------------------------------------------------
void RenderSequence::OnRender(Pass* pass, PassInfo* passInfo)
{
    mObjects[mActiveObjectIndex]->OnRender(pass, passInfo);
}
//----------------------------------------------------------------------------
void RenderSequence::OnEnableBuffers()
{
    mObjects[mActiveObjectIndex]->OnEnableBuffers();
}
//----------------------------------------------------------------------------
void RenderSequence::OnDisableBuffers()
{
    mObjects[mActiveObjectIndex]->OnDisableBuffers();
}
//----------------------------------------------------------------------------
void RenderSequence::SetActiveRenderObject(unsigned int index)
{
    RTGI_ASSERT(index >= 0 && index < mObjects.size());

    mCamera = mObjects[index]->GetCamera();
    mMaterial = mObjects[index]->GetMaterial();
    mPrimitive = mObjects[index]->GetPrimitive();
    mSpatialInfo = mObjects[index]->GetSpatialInfo();
    mRenderCache = mObjects[index]->GetRenderCache();
}
//----------------------------------------------------------------------------
void RenderSequence::AddRenderObject(RenderObject* renderObject)
{
    mObjects.push_back(renderObject);
}
//----------------------------------------------------------------------------