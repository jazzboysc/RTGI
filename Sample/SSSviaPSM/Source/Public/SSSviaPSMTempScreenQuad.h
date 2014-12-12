#ifndef RTGI_SSSviaPSMTempScreenQuad_H
#define RTGI_SSSviaPSMTempScreenQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/09/2014
//----------------------------------------------------------------------------
class SSSviaPSMTempScreenQuad : public ScreenQuad
{
public:
    SSSviaPSMTempScreenQuad(Material* material);
    virtual ~SSSviaPSMTempScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	Texture2DPtr TempTexture;

private:
	ShaderUniform mTempSamplerLoc;
};

typedef RefPointer<SSSviaPSMTempScreenQuad> SSSviaPSMTempScreenQuadPtr;

}

#endif