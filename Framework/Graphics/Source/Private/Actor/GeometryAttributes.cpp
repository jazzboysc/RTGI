//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "GeometryAttributes.h"

using namespace RTGI;

//----------------------------------------------------------------------------
GeometryAttributes::GeometryAttributes()
{
    Prim = 0;
	HasNormal = false;
	HasTCoord = false;
	VertexComponentCount = false;
}
//----------------------------------------------------------------------------
GeometryAttributes::~GeometryAttributes()
{
}
//----------------------------------------------------------------------------