//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_RefObject_H
#define RTGI_RefObject_H

#include "RefPointer.h"
#include "RTGIAssert.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/14/2013
//----------------------------------------------------------------------------
class RefObject
{
public:
    virtual ~RefObject();

// Smart pointer system.
public:
    inline void IncrementReferenceCount();
    void DecrementReferenceCount();
    inline int GetReferenceCount() const;

protected:
    // Abstract base class.
    RefObject();

private:
    int mReferenceCount;
};

typedef RefPointer<RefObject> RefObjectPtr;

#include "RefObject.inl"

}

#endif