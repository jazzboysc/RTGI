#ifndef RTGI_VPLTempScreenQuad_H
#define RTGI_VPLTempScreenQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/21/2014
//----------------------------------------------------------------------------
class VPLTempScreenQuad : public ScreenQuad
{
public:
    VPLTempScreenQuad(Material* material);
    virtual ~VPLTempScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	Texture2DPtr TempTexture;

private:
	GLint mTempSamplerLoc;
};

typedef RefPointer<VPLTempScreenQuad> VPLTempScreenQuadPtr;

}

#endif