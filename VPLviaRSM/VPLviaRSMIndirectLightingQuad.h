#ifndef RTGI_VPLviaRSMIndirectLightingQuad_H
#define RTGI_VPLviaRSMIndirectLightingQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/15/2014
//----------------------------------------------------------------------------
class VPLviaRSMIndirectLightingQuad : public ScreenQuad
{
public:
    VPLviaRSMIndirectLightingQuad(Material* material);
    virtual ~VPLviaRSMIndirectLightingQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

    Texture2DPtr GBufferPositionTexture;
    Texture2DPtr GBufferNormalTexture;
    Texture2DPtr GBufferDiffuseTexture;
	Texture2DPtr VPLShadowMap;

    Camera* VPLProjector;
    vec3 VPLColor;

private:
    GLint mVPLProjectorViewLoc;
    GLint mVPLPositionWorldLoc;
    GLint mVPLNormalWorldLoc;
    GLint mVPLColorLoc;
    GLint mVPLProjectorNearFarLoc;

    GLint mGBufferPositionSamplerLoc;
    GLint mGBufferNormalSamplerLoc;
    GLint mGBufferDiffuseSamplerLoc;
	GLint mVPLShadowMapSamplerLoc;
};

typedef RefPointer<VPLviaRSMIndirectLightingQuad> VPLviaRSMIndirectLightingQuadPtr;

}

#endif