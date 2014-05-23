//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "RefObject.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RefObject::RefObject()
{
	mReferenceCount = 0;
}
//----------------------------------------------------------------------------
RefObject::~RefObject()
{
}
//----------------------------------------------------------------------------
void RefObject::DecrementReferenceCount()
{
    if( --mReferenceCount == 0 )
    {
        delete this;
    }
}
//----------------------------------------------------------------------------