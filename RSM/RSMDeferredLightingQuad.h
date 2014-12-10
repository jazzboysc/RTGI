#ifndef RTGI_RSMDeferredLightingQuad_H
#define RTGI_RSMDeferredLightingQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"
#include "Light.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 06/26/2014
//----------------------------------------------------------------------------
class RSMDeferredLightingQuad : public ScreenQuad
{
public:
	RSMDeferredLightingQuad(Material* material,  Light* light);
	virtual ~RSMDeferredLightingQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	Texture2DPtr PositionTexture;
	Texture2DPtr NormalTexture;
	Texture2DPtr ReflectanceTexture;
    Texture2DPtr IndirectLightingTexture;

private:
	Light* mLight;

	ShaderUniform mLightPositionLoc;
    ShaderUniform mLightColorLoc;
    ShaderUniform mPositionSamplerLoc;
    ShaderUniform mNormalSamplerLoc;
    ShaderUniform mReflectanceSamplerLoc;
    ShaderUniform mIndirectLightingSamplerLoc;
};

typedef RefPointer<RSMDeferredLightingQuad> RSMDeferredLightingQuadPtr;

}

#endif