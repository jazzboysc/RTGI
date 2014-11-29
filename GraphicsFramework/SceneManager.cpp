//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "SceneManager.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SceneManager::SceneManager()
{
}
//----------------------------------------------------------------------------
SceneManager::~SceneManager()
{
}
//----------------------------------------------------------------------------
void SceneManager::AddRenderObject(RenderObject* renderObject)
{
    mRenderObjects.push_back(renderObject);
}
//----------------------------------------------------------------------------
int SceneManager::GetRenderObjectCount() const
{
    return (int)mRenderObjects.size();
}
//----------------------------------------------------------------------------
RenderObject* SceneManager::GetRenderObject(int i) const
{
    assert( i >= 0 && i < (int)mRenderObjects.size() );
    return mRenderObjects[i];
}
//----------------------------------------------------------------------------