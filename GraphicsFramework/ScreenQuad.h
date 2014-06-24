//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_ScreenQuad_H
#define RTGI_ScreenQuad_H

#include "FrameworkCommon.h"
#include "TriangleMesh.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/23/2014
//----------------------------------------------------------------------------
class ScreenQuad : public TriangleMesh
{
public:
	ScreenQuad(Material* material, Camera* camera = 0);
	virtual ~ScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();
};

typedef RefPointer<ScreenQuad> ScreenQuadPtr;

}

#endif