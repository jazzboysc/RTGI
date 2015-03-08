//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Cacheable.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Cacheable::Cacheable()
    :
    mCacheFlag(CF_Nothing),
    mCacheSize(0),
    mWorld(0),
    mView(0),
    mProj(0),
    mCache(0)
{
}
//----------------------------------------------------------------------------
Cacheable::~Cacheable()
{
    delete[] mCache;
}
//----------------------------------------------------------------------------
void Cacheable::SetCacheFlag(Cacheable::CacheFlag flag)
{
    if( mCache )
    {
        delete[] mCache;
        mCache = 0;
        mCacheSize = 0;
    }

    mCacheFlag = flag;
    if( mCacheFlag & Cacheable::CacheFlag::CF_WorldTransform )
    {
        mCacheSize += sizeof(glm::mat4);
    }

    if( mCacheFlag & Cacheable::CacheFlag::CF_ViewTransform )
    {
        mCacheSize += sizeof(glm::mat4);
    }

    if( mCacheFlag & Cacheable::CacheFlag::CF_ProjTransform )
    {
        mCacheSize += sizeof(glm::mat4);
    }

    if( mCacheSize > 0 )
    {
        mCache = new unsigned char[mCacheSize];
        memset(mCache, 0, mCacheSize);

        glm::mat4* data = (glm::mat4*)mCache;
        if( mCacheFlag & Cacheable::CacheFlag::CF_WorldTransform )
        {
            mWorld = data++;
        }

        if( mCacheFlag & Cacheable::CacheFlag::CF_ViewTransform )
        {
            mView = data++;
        }

        if( mCacheFlag & Cacheable::CacheFlag::CF_ProjTransform )
        {
            mProj = data++;
        }
    }
}
//----------------------------------------------------------------------------