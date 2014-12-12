#ifndef RTGI_DirectLightingQuad_H
#define RTGI_DirectLightingQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"
#include "Light.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/24/2014
//----------------------------------------------------------------------------
class DirectLightingQuad : public ScreenQuad
{
public:
	DirectLightingQuad(Material* material,  Light* light);
	virtual ~DirectLightingQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	Texture2DPtr PositionTexture;
	Texture2DPtr NormalTexture;
	Texture2DPtr ReflectanceTexture;

private:
	Light* mLight;

	ShaderUniform mLightPositionLoc;
    ShaderUniform mLightColorLoc;
    ShaderUniform mPositionSamplerLoc;
    ShaderUniform mNormalSamplerLoc;
    ShaderUniform mReflectanceSamplerLoc;
};

typedef RefPointer<DirectLightingQuad> DirectLightingQuadPtr;

}

#endif