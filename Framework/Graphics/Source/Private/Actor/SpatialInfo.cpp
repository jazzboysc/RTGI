//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "SpatialInfo.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SpatialInfo::SpatialInfo()
    :
    mWorldScale(1.0f, 1.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
SpatialInfo::~SpatialInfo()
{
}
//----------------------------------------------------------------------------
void SpatialInfo::SetWorldTransform(const glm::mat4& worldTrans)
{
    mWorldTransform = worldTrans;
    mWorldTransform[0][0] *= mWorldScale[0];
    mWorldTransform[0][1] *= mWorldScale[1];
    mWorldTransform[0][2] *= mWorldScale[2];
    mWorldTransform[1][0] *= mWorldScale[0];
    mWorldTransform[1][1] *= mWorldScale[1];
    mWorldTransform[1][2] *= mWorldScale[2];
    mWorldTransform[2][0] *= mWorldScale[0];
    mWorldTransform[2][1] *= mWorldScale[1];
    mWorldTransform[2][2] *= mWorldScale[2];
}
//----------------------------------------------------------------------------
void SpatialInfo::SetWorldTranslation(const glm::vec3& translation)
{
    mWorldTransform[3][0] = translation.x;
    mWorldTransform[3][1] = translation.y;
    mWorldTransform[3][2] = translation.z;
}
//----------------------------------------------------------------------------
void SpatialInfo::SetWorldScale(const glm::vec3& scale)
{
    mWorldTransform[0][0] /= mWorldScale[0];
    mWorldTransform[0][1] /= mWorldScale[1];
    mWorldTransform[0][2] /= mWorldScale[2];
    mWorldTransform[1][0] /= mWorldScale[0];
    mWorldTransform[1][1] /= mWorldScale[1];
    mWorldTransform[1][2] /= mWorldScale[2];
    mWorldTransform[2][0] /= mWorldScale[0];
    mWorldTransform[2][1] /= mWorldScale[1];
    mWorldTransform[2][2] /= mWorldScale[2];
    mWorldTransform[0][0] *= scale[0];
    mWorldTransform[0][1] *= scale[1];
    mWorldTransform[0][2] *= scale[2];
    mWorldTransform[1][0] *= scale[0];
    mWorldTransform[1][1] *= scale[1];
    mWorldTransform[1][2] *= scale[2];
    mWorldTransform[2][0] *= scale[0];
    mWorldTransform[2][1] *= scale[1];
    mWorldTransform[2][2] *= scale[2];
    mWorldScale = scale;
}
//----------------------------------------------------------------------------