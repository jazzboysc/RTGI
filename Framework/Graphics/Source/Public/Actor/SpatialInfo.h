//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_SpatialInfo_H
#define RTGI_SpatialInfo_H

#include "RefObject.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 03/08/2015
//----------------------------------------------------------------------------
class SpatialInfo : public RefObject
{
public:
    SpatialInfo();
    ~SpatialInfo();

    void SetWorldTransform(const glm::mat4& worldTrans);
    inline glm::mat4 GetWorldTransform() const;
    void SetWorldTranslation(const glm::vec3& translation);
    inline glm::vec3 GetWorldTranslation() const;
    void SetWorldScale(const glm::vec3& scale);
    inline glm::vec3 GetWorldScale() const;

    glm::mat4 Offset;

private:
    friend class RenderCache;
    glm::mat4 mWorldTransform;
    glm::vec3 mWorldScale;
};

typedef RefPointer<SpatialInfo> SpatialInfoPtr;

#include "SpatialInfo.inl"

}

#endif