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
    RTGI_ASSERT(mSpatialInfo);
    mSpatialInfo->SetWorldTransform(worldTrans);
}
//----------------------------------------------------------------------------
glm::mat4 RenderObject::GetWorldTransform() const
{
    RTGI_ASSERT(mSpatialInfo);
    return mSpatialInfo->GetWorldTransform();
}
//----------------------------------------------------------------------------
void RenderObject::SetWorldTranslation(const glm::vec3& translation)
{
    RTGI_ASSERT(mSpatialInfo);
    mSpatialInfo->SetWorldTranslation(translation);
}
//----------------------------------------------------------------------------
glm::vec3 RenderObject::GetWorldTranslation() const
{
    RTGI_ASSERT(mSpatialInfo);
    return mSpatialInfo->GetWorldTranslation();
}
//----------------------------------------------------------------------------
void RenderObject::SetWorldScale(const glm::vec3& scale)
{
    RTGI_ASSERT(mSpatialInfo);
    mSpatialInfo->SetWorldScale(scale);
}
//----------------------------------------------------------------------------
glm::vec3 RenderObject::GetWorldScale() const
{
    RTGI_ASSERT(mSpatialInfo);
    return mSpatialInfo->GetWorldScale();
}
//----------------------------------------------------------------------------