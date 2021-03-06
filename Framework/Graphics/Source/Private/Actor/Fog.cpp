//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Fog.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Fog::Fog()
	:
	Color(1.0f, 1.0f, 1.0f),
	Range(0.0f, 1.0f),
	IsExponential(false)
{
}
//----------------------------------------------------------------------------
Fog::~Fog()
{
}
//----------------------------------------------------------------------------