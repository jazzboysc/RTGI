//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "GeometryAttributes.h"

using namespace RTGI;

//----------------------------------------------------------------------------
GeometryAttributes::GeometryAttributes()
{
	VBO = 0;
	IBO = 0;
	HasNormal = false;
	HasTCoord = false;
	VertexComponentCount = false;
}
//----------------------------------------------------------------------------
GeometryAttributes::~GeometryAttributes()
{
}
//----------------------------------------------------------------------------