//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_AABB_H
#define RTGI_AABB_H

#include "FrameworkCommon.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 10/02/2013
//----------------------------------------------------------------------------
class AABB
{
public:
	AABB();
	~AABB();

	float GetDiagonal();
	vec3 GetBoxCenter();

	vec3 Min;
	vec3 Max;
};

}

#endif