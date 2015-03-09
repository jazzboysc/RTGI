//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void RenderObject::SetSpatialInfo(SpatialInfo* spatialInfo)
{
    mSpatialInfo = spatialInfo;
}
//----------------------------------------------------------------------------
SpatialInfo* RenderObject::GetSpatialInfo()
{
    return mSpatialInfo;
}
//----------------------------------------------------------------------------
void RenderObject::SetWorldTransform(const glm::mat4& worldTrans)
{
    assert(mSpatialInfo);
    mSpatialInfo->SetWorldTransform(worldTrans);
}
//----------------------------------------------------------------------------
glm::mat4 RenderObject::GetWorldTransform() const
{
    assert(mSpatialInfo);
    return mSpatialInfo->GetWorldTransform();
}
//----------------------------------------------------------------------------
void RenderObject::SetWorldTranslation(const glm::vec3& translation)
{
    assert(mSpatialInfo);
    mSpatialInfo->SetWorldTranslation(translation);
}
//----------------------------------------------------------------------------
glm::vec3 RenderObject::GetWorldTranslation() const
{
    assert(mSpatialInfo);
    return mSpatialInfo->GetWorldTranslation();
}
//----------------------------------------------------------------------------
void RenderObject::SetWorldScale(const glm::vec3& scale)
{
    assert(mSpatialInfo);
    mSpatialInfo->SetWorldScale(scale);
}
//----------------------------------------------------------------------------
glm::vec3 RenderObject::GetWorldScale() const
{
    assert(mSpatialInfo);
    return mSpatialInfo->GetWorldScale();
}
//----------------------------------------------------------------------------
void RenderObject::SetRenderCache(RenderCache* renderCache)
{
    mRenderCache = renderCache;
}
//----------------------------------------------------------------------------
RenderCache* RenderObject::GetRenderCache()
{
    return mRenderCache;
}
//----------------------------------------------------------------------------
void RenderObject::UpdateRenderCache()
{
    assert(mRenderCache && mSpatialInfo);
    mRenderCache->Update(mSpatialInfo);
}
//----------------------------------------------------------------------------