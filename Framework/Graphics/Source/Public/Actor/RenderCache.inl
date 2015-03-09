//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
RenderCache::CacheFlag RenderCache::GetCacheFlag() const
{
    return mCacheFlag;
}
//----------------------------------------------------------------------------
unsigned int RenderCache::GetCacheSize() const
{
    return mCacheSize;
}
//----------------------------------------------------------------------------
glm::mat4* RenderCache::GetWorldCache() const
{
    return mWorldCache;
}
//----------------------------------------------------------------------------
glm::mat4* RenderCache::GetViewCache() const
{
    return mViewCache;
}
//----------------------------------------------------------------------------
glm::mat4* RenderCache::GetProjCache() const
{
    return mProjCache;
}
//----------------------------------------------------------------------------