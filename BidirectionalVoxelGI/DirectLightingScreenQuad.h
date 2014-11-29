#ifndef RTGI_DirectLightingScreenQuad_H
#define RTGI_DirectLightingScreenQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
class DirectLightingScreenQuad : public ScreenQuad
{
public:
    DirectLightingScreenQuad(Material* material);
    virtual ~DirectLightingScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

    Texture2DPtr GBufferPositionTexture;
    Texture2DPtr GBufferNormalTexture;
    Texture2DPtr GBufferAlbedoTexture;
    Texture2DPtr ShadowMapTexture;
    Camera* LightProjector;
    vec3 LightColor;

private:
	GLint mGBufferPositionSamplerLoc;
    GLint mGBufferNormalSamplerLoc;
    GLint mGBufferAlbedoSamplerLoc;
    GLint mShadowMapSamplerLoc;
    GLint mLightProjectorViewLoc;
    GLint mLightPositionWorldLoc;
    GLint mLightColorLoc;
    GLint mLightProjectorNearFarLoc;
};

typedef RefPointer<DirectLightingScreenQuad> DirectLightingScreenQuadPtr;

}

#endif