//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
glm::mat4 Camera::GetViewCacheTransform()
{
    assert(mRenderCache);
    return *mRenderCache->GetViewCache();
}
//----------------------------------------------------------------------------
glm::mat4 Camera::GetProjectionCacheTransform()
{
    assert(mRenderCache);
    return *mRenderCache->GetProjCache();
}
//----------------------------------------------------------------------------
void Camera::SetRenderCache(RenderCache* renderCache)
{
    mRenderCache = renderCache;
}
//----------------------------------------------------------------------------
RenderCache* Camera::GetRenderCache()
{
    return mRenderCache;
}
//----------------------------------------------------------------------------
void Camera::UpdateRenderCache()
{
    assert(mRenderCache);
    mRenderCache->Update(this);
}
//----------------------------------------------------------------------------