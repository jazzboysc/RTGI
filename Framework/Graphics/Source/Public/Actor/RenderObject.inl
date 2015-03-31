//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void RenderObject::SetSpatialInfo(SpatialInfo* spatialInfo)
{
    mSpatialInfo = spatialInfo;
}
//----------------------------------------------------------------------------
SpatialInfo* RenderObject::GetSpatialInfo()
{
    return mSpatialInfo;
}
//----------------------------------------------------------------------------