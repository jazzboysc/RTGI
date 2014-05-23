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
	virtual void OnUpdateShaderConstants();
	virtual void OnGetShaderConstants();

	Texture2DPtr PositionTexture;
	Texture2DPtr NormalTexture;
	Texture2DPtr ReflectanceTexture;

private:
	Light* mLight;

	GLint mLightPositionLoc;
	GLint mLightColorLoc;
	GLint mPositionSamplerLoc;
	GLint mNormalSamplerLoc;
	GLint mReflectanceSamplerLoc;
};

typedef RefPointer<DirectLightingQuad> DirectLightingQuadPtr;

}

#endif