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
    inline CacheFlag GetCacheFlag() const { return mCacheFlag; }
    inline unsigned int GetCacheSize() const { return mCacheSize; }

protected:
    CacheFlag mCacheFlag;
    unsigned int mCacheSize;
    glm::mat4* mWorld;
    glm::mat4* mView;
    glm::mat4* mProj;
    unsigned char* mCache;
};

typedef RefPointer<Cacheable> CacheablePtr;

}

#endif