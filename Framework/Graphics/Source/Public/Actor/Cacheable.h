//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Cacheable_H
#define RTGI_Cacheable_H

#include "RefObject.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 03/07/2015
//----------------------------------------------------------------------------
class Cacheable : public RefObject
{
public:
    enum CacheFlag
    {
        CF_Nothing        = 0,
        CF_WorldTransform = 1,
        CF_ViewTransform  = 2,
        CF_ProjTransform  = 4
    };

    Cacheable();
    virtual ~Cacheable();

    void SetCacheFlag(CacheFlag flag);

    inline CacheFlag GetCacheFlag() const;
    inline unsigned int GetCacheSize() const;
    inline glm::mat4* GetWorldCache() const;
    inline glm::mat4* GetViewCache() const;
    inline glm::mat4* GetProjCache() const;

protected:
    CacheFlag mCacheFlag;
    unsigned int mCacheSize;
    glm::mat4* mWorldCache;
    glm::mat4* mViewCache;
    glm::mat4* mProjCache;
    unsigned char* mCache;
};

typedef RefPointer<Cacheable> CacheablePtr;

#include "Cacheable.inl"

}

#endif