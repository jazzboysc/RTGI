//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
Cacheable::CacheFlag Cacheable::GetCacheFlag() const
{
    return mCacheFlag;
}
//----------------------------------------------------------------------------
unsigned int Cacheable::GetCacheSize() const
{
    return mCacheSize;
}
//----------------------------------------------------------------------------
glm::mat4* Cacheable::GetWorldCache() const
{
    return mWorldCache;
}
//----------------------------------------------------------------------------
glm::mat4* Cacheable::GetViewCache() const
{
    return mViewCache;
}
//----------------------------------------------------------------------------
glm::mat4* Cacheable::GetProjCache() const
{
    return mProjCache;
}
//----------------------------------------------------------------------------