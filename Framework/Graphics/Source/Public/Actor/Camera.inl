//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

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