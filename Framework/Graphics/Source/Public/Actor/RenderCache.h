//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_RenderCache_H
#define RTGI_RenderCache_H

#include "RefObject.h"
#include "SpatialInfo.h"

namespace RTGI
{

class Camera;
//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 03/07/2015
//----------------------------------------------------------------------------
class RenderCache : public RefObject
{
public:
    enum CacheFlag
    {
        CF_Nothing        = 0,
        CF_WorldTransform = 1,
        CF_ViewTransform  = 2,
        CF_ProjTransform  = 4
    };

    RenderCache();
    virtual ~RenderCache();

    void SetCacheFlag(CacheFlag flag);

    inline CacheFlag GetCacheFlag() const;
    inline unsigned int GetCacheSize() const;
    inline glm::mat4* GetWorldCache() const;
    inline glm::mat4* GetViewCache() const;
    inline glm::mat4* GetProjCache() const;

    void Update(SpatialInfo* spatialInfo);
    void Update(Camera* camera);

protected:
    CacheFlag mCacheFlag;
    unsigned int mCacheSize;
    glm::mat4* mWorldCache;
    glm::mat4* mViewCache;
    glm::mat4* mProjCache;
    unsigned char* mCache;
};

typedef RefPointer<RenderCache> RenderCachePtr;

#include "RenderCache.inl"

}

#endif