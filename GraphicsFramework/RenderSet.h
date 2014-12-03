//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_RenderSet_H
#define RTGI_RenderSet_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "RenderObject.h"
#include <vector>

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2013
//----------------------------------------------------------------------------
class RenderSet : public RefObject
{
public:
    RenderSet();
    virtual ~RenderSet();

    void AddRenderObject(RenderObject* renderObject);
    int GetRenderObjectCount() const;
    RenderObject* GetRenderObject(int i) const;

protected:
    std::vector<RenderObjectPtr> mRenderObjects;
};

typedef RefPointer<RenderSet> RenderSetPtr;

}

#endif