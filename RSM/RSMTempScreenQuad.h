#ifndef RTGI_RSMTempScreenQuad_H
#define RTGI_RSMTempScreenQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 06/15/2014
//----------------------------------------------------------------------------
class RSMTempScreenQuad : public ScreenQuad
{
public:
	RSMTempScreenQuad(Material* material);
	virtual ~RSMTempScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants();
	virtual void OnGetShaderConstants();

	Texture2DPtr TempTexture;

private:
	GLint mTempSamplerLoc;
};

typedef RefPointer<RSMTempScreenQuad> RSMTempScreenQuadPtr;

}

#endif