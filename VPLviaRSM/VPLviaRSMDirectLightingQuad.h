#ifndef RTGI_VPLviaRSMDirectLightingQuad_H
#define RTGI_VPLviaRSMDirectLightingQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/15/2014
//----------------------------------------------------------------------------
class VPLviaRSMDirectLightingQuad : public ScreenQuad
{
public:
    VPLviaRSMDirectLightingQuad(Material* material);
    virtual ~VPLviaRSMDirectLightingQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

    Texture2DPtr GBufferPositionTexture;
    Texture2DPtr GBufferNormalTexture;
    Texture2DPtr GBufferDiffuseTexture;
	Texture2DPtr ShadowMap;

    Camera* LightProjector;

private:
    GLint mLightProjectorViewLoc;
    GLint mLightPositionWorldLoc;
    GLint mLightColorLoc;
    GLint mLightProjectorNearFarLoc;

    GLint mGBufferPositionSamplerLoc;
    GLint mGBufferNormalSamplerLoc;
    GLint mGBufferDiffuseSamplerLoc;
	GLint mShadowMapSamplerLoc;
};

typedef RefPointer<VPLviaRSMDirectLightingQuad> VPLviaRSMDirectLightingQuadPtr;

}

#endif