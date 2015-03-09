//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "RenderCache.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RenderCache::RenderCache()
    :
    mCacheFlag(CF_Nothing),
    mCacheSize(0),
    mWorldCache(0),
    mViewCache(0),
    mProjCache(0),
    mCache(0)
{
}
//----------------------------------------------------------------------------
RenderCache::~RenderCache()
{
    delete[] mCache;
}
//----------------------------------------------------------------------------
void RenderCache::SetCacheFlag(RenderCache::CacheFlag flag)
{
    if( mCache )
    {
        delete[] mCache;
        mCache = 0;
        mCacheSize = 0;
    }

    mCacheFlag = flag;
    if( mCacheFlag & RenderCache::CacheFlag::CF_WorldTransform )
    {
        mCacheSize += sizeof(glm::mat4);
    }

    if( mCacheFlag & RenderCache::CacheFlag::CF_ViewTransform )
    {
        mCacheSize += sizeof(glm::mat4);
    }

    if( mCacheFlag & RenderCache::CacheFlag::CF_ProjTransform )
    {
        mCacheSize += sizeof(glm::mat4);
    }

    if( mCacheSize > 0 )
    {
        mCache = new unsigned char[mCacheSize];
        memset(mCache, 0, mCacheSize);

        glm::mat4* data = (glm::mat4*)mCache;
        if( mCacheFlag & RenderCache::CacheFlag::CF_WorldTransform )
        {
            mWorldCache = data++;
        }

        if( mCacheFlag & RenderCache::CacheFlag::CF_ViewTransform )
        {
            mViewCache = data++;
        }

        if( mCacheFlag & RenderCache::CacheFlag::CF_ProjTransform )
        {
            mProjCache = data++;
        }
    }
}
//----------------------------------------------------------------------------
void RenderCache::Update(SpatialInfo* spatialInfo)
{
    *mWorldCache = spatialInfo->mWorldTransform;
}
//----------------------------------------------------------------------------