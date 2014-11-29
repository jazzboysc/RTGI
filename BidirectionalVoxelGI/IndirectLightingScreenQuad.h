#ifndef RTGI_IndirectLightingScreenQuad_H
#define RTGI_IndirectLightingScreenQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"
#include "StructuredBuffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
class IndirectLightingScreenQuad : public ScreenQuad
{
public:
    IndirectLightingScreenQuad(Material* material);
    virtual ~IndirectLightingScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

    int VPLCount;
    float BounceSingularity;
    Texture2DPtr GBufferPositionTexture;
    Texture2DPtr GBufferNormalTexture;
    Texture2DPtr GBufferAlbedoTexture;
    StructuredBufferPtr VPLBuffer;

private:
    GLint mVPLCountLoc;
    GLint mBounceSingularityLoc;
	GLint mGBufferPositionSamplerLoc;
    GLint mGBufferNormalSamplerLoc;
    GLint mGBufferAlbedoSamplerLoc;
};

typedef RefPointer<IndirectLightingScreenQuad> IndirectLightingScreenQuadPtr;

}

#endif