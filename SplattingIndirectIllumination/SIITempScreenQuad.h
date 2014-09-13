#ifndef RTGI_SIITempScreenQuad_H
#define RTGI_SIITempScreenQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/12/2014
//----------------------------------------------------------------------------
class SIITempScreenQuad : public ScreenQuad
{
public:
    SIITempScreenQuad(Material* material);
    virtual ~SIITempScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	Texture2DPtr TempTexture;

private:
	GLint mTempSamplerLoc;
};

typedef RefPointer<SIITempScreenQuad> SIITempScreenQuadPtr;

}

#endif