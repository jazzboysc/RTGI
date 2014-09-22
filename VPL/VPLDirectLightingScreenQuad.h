#ifndef RTGI_VPLDirectLightingScreenQuad_H
#define RTGI_VPLDirectLightingScreenQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/21/2014
//----------------------------------------------------------------------------
class VPLDirectLightingScreenQuad : public ScreenQuad
{
public:
    VPLDirectLightingScreenQuad(Material* material);
    virtual ~VPLDirectLightingScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

    Texture2DPtr GBufferPositionTexture;
    Texture2DPtr GBufferNormalTexture;
    Texture2DPtr GBufferAlbedoTexture;
    Texture2DPtr ShadowMapTexture;
    Camera* LightProjector;

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

typedef RefPointer<VPLDirectLightingScreenQuad> VPLDirectLightingScreenQuadPtr;

}

#endif