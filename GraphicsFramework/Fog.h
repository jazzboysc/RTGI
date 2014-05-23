//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Fog_H
#define RTGI_Fog_H

#include "FrameworkCommon.h"
#include "RefObject.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/15/2013
//----------------------------------------------------------------------------
class Fog : public RefObject
{
public:
	Fog();
    ~Fog();

	vec3 Color;
	vec2 Range;
	bool IsExponential;
};

typedef RefPointer<Fog> FogPtr;

}

#endif
