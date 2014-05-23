//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Light_H
#define RTGI_Light_H

#include "FrameworkCommon.h"
#include "RefObject.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/14/2013
//----------------------------------------------------------------------------
class Light : public RefObject
{
public:
	Light();
    virtual ~Light();

	vec3 Location;
	vec3 Color;
};

typedef RefPointer<Light> LightPtr;

}

#endif
