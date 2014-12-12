#ifndef RTGI_SSDOTempScreenQuad_H
#define RTGI_SSDOTempScreenQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/23/2014
//----------------------------------------------------------------------------
class SSDOTempScreenQuad : public ScreenQuad
{
public:
	SSDOTempScreenQuad(Material* material);
	virtual ~SSDOTempScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	Texture2DPtr TempTexture;

private:
	ShaderUniform mTempSamplerLoc;
};

typedef RefPointer<SSDOTempScreenQuad> SSDOTempScreenQuadPtr;

}

#endif