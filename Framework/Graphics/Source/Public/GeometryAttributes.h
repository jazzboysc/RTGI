//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_GeometryAttributes_H
#define RTGI_GeometryAttributes_H

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 08/09/2014
//----------------------------------------------------------------------------
class GeometryAttributes
{
public:
	GeometryAttributes();
	~GeometryAttributes();

	GLuint VBO;
	GLuint IBO;
	bool HasNormal;
	bool HasTCoord;
	int VertexComponentCount;
};

}

#endif