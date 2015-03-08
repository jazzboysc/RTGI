//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
glm::mat4 SpatialInfo::GetWorldTransform() const
{
    return mWorldTransform;
}
//----------------------------------------------------------------------------
glm::vec3 SpatialInfo::GetWorldTranslation() const
{
    glm::vec3 res;
    res.x = mWorldTransform[3][0];
    res.y = mWorldTransform[3][1];
    res.z = mWorldTransform[3][2];
    return res;
}
//----------------------------------------------------------------------------
glm::vec3 SpatialInfo::GetWorldScale() const
{
    return mWorldScale;
}
//----------------------------------------------------------------------------