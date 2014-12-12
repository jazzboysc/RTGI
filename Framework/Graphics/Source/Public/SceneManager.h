//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_SceneManager_H
#define RTGI_SceneManager_H


#include "RefObject.h"
#include "RenderObject.h"
#include <vector>

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2013
//----------------------------------------------------------------------------
class SceneManager : public RefObject
{
public:
    SceneManager();
    virtual ~SceneManager();

    void AddRenderObject(RenderObject* renderObject);
    int GetRenderObjectCount() const;
    RenderObject* GetRenderObject(int i) const;

protected:
    std::vector<RenderObjectPtr> mRenderObjects;
};

typedef RefPointer<SceneManager> SceneManagerPtr;

}

#endif