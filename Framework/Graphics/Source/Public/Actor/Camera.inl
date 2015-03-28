//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
glm::mat4 Camera::GetViewCacheTransform()
{
    RTGI_ASSERT(mRenderCache);
    return *mRenderCache->GetViewCache();
}
//----------------------------------------------------------------------------
glm::mat4 Camera::GetProjectionCacheTransform()
{
    RTGI_ASSERT(mRenderCache);
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
    RTGI_ASSERT(mRenderCache);
    mRenderCache->Update(this);
}
//----------------------------------------------------------------------------