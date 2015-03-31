//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "RenderSequence.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RenderSequence::RenderSequence(Material* material)
    :
    RenderObject(material)
{
    mActiveObjectIndex = 0;
    mFrequence = 1.0f;
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
void RenderSequence::SetCamera(Camera* camera)
{
    mObjects[mActiveObjectIndex]->SetCamera(camera);
}
//----------------------------------------------------------------------------
Camera* RenderSequence::GetCamera() const
{
    return mObjects[mActiveObjectIndex]->GetCamera();
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
void RenderSequence::SetWorldTransform(const glm::mat4& worldTrans)
{
    for( int i = 0; i < (int)mObjects.size(); ++i )
    {
        mObjects[i]->SetWorldTransform(worldTrans);
    }
}
//----------------------------------------------------------------------------
glm::mat4 RenderSequence::GetWorldTransform() const
{
    return mObjects[mActiveObjectIndex]->GetWorldTransform();
}
//----------------------------------------------------------------------------
void RenderSequence::SetWorldTranslation(const glm::vec3& translation)
{
    for( int i = 0; i < (int)mObjects.size(); ++i )
    {
        mObjects[i]->SetWorldTranslation(translation);
    }
}
//----------------------------------------------------------------------------
glm::vec3 RenderSequence::GetWorldTranslation() const
{
    return mObjects[mActiveObjectIndex]->GetWorldTranslation();
}
//----------------------------------------------------------------------------
void RenderSequence::SetWorldScale(const glm::vec3& scale)
{
    for( int i = 0; i < (int)mObjects.size(); ++i )
    {
        mObjects[i]->SetWorldScale(scale);
    }
}
//----------------------------------------------------------------------------
glm::vec3 RenderSequence::GetWorldScale() const
{
    return mObjects[mActiveObjectIndex]->GetWorldScale();
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
void RenderSequence::Update(unsigned int frame)
{
    int sequenceSize = (int)mObjects.size();
    SetActiveRenderObject(unsigned int((float)frame*mFrequence) % 
        sequenceSize);
}
//----------------------------------------------------------------------------
void RenderSequence::SetActiveRenderObject(unsigned int index)
{
    RTGI_ASSERT(index >= 0 && index < mObjects.size());
    mActiveObjectIndex = index;
}
//----------------------------------------------------------------------------
void RenderSequence::SetFrequence(float value)
{
    RTGI_ASSERT(value > 0.0f);
    mFrequence = value;
}
//----------------------------------------------------------------------------
void RenderSequence::AddRenderObject(RenderObject* renderObject)
{
    mObjects.push_back(renderObject);
}
//----------------------------------------------------------------------------