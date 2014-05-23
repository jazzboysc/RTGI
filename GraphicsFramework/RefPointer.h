//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_RefPointer_H
#define RTGI_RefPointer_H

#include "FrameworkCommon.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/14/2013
// This class is based on Wild Magic 4's smart pointer.
// http://www.geometrictools.com
//----------------------------------------------------------------------------
template <class T>
class RefPointer
{
public:
    RefPointer(T* object = 0);
	RefPointer(const RefPointer& refPointer);
    ~RefPointer();

    operator T* () const;
    T& operator * () const;
    T* operator -> () const;

    RefPointer& operator = (T* object);
    RefPointer& operator = (const RefPointer& refPointer);

    bool operator == (T* pObject) const;
    bool operator != (T* pObject) const;
    bool operator == (const RefPointer& refPointer) const;
    bool operator != (const RefPointer& refPointer) const;

protected:
    T* mObject;
};

#include "RefPointer.inl"

}

#endif