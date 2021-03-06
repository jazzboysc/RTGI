//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "RenderSet.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RenderSet::RenderSet()
{
}
//----------------------------------------------------------------------------
RenderSet::~RenderSet()
{
}
//----------------------------------------------------------------------------
void RenderSet::AddRenderObject(RenderObject* renderObject)
{
    mRenderObjects.push_back(renderObject);
}
//----------------------------------------------------------------------------
int RenderSet::GetRenderObjectCount() const
{
    return (int)mRenderObjects.size();
}
//----------------------------------------------------------------------------
RenderObject* RenderSet::GetRenderObject(int i) const
{
    RTGI_ASSERT( i >= 0 && i < (int)mRenderObjects.size() );
    return mRenderObjects[i];
}
//----------------------------------------------------------------------------
void RenderSet::UpdateRenderCache()
{
    for( int i = 0; i < (int)mRenderObjects.size(); ++i )
    {
        mRenderObjects[i]->UpdateRenderCache();
    }
}
//----------------------------------------------------------------------------