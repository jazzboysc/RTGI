#ifndef RTGI_VPLviaRSMFinalScreenQuad_H
#define RTGI_VPLviaRSMFinalScreenQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/14/2014
//----------------------------------------------------------------------------
class VPLviaRSMFinalScreenQuad : public ScreenQuad
{
public:
    VPLviaRSMFinalScreenQuad(Material* material);
    virtual ~VPLviaRSMFinalScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	Texture2DPtr DirectLightingTexture;
    Texture2DPtr IndirectLightingTexture;

private:
	GLint mDirectLightingSamplerLoc;
    GLint mIndirectLightingSamplerLoc;
};

typedef RefPointer<VPLviaRSMFinalScreenQuad> VPLviaRSMFinalScreenQuadPtr;

}

#endif