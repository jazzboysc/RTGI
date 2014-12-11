#ifndef RTGI_VPLIndirectLightingScreenQuad_H
#define RTGI_VPLIndirectLightingScreenQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"
#include "StructuredBuffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/26/2014
//----------------------------------------------------------------------------
class VPLIndirectLightingScreenQuad : public ScreenQuad
{
public:
    VPLIndirectLightingScreenQuad(Material* material);
    virtual ~VPLIndirectLightingScreenQuad();

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
    ShaderUniform mVPLCountLoc;
    ShaderUniform mBounceSingularityLoc;
    ShaderUniform mGBufferPositionSamplerLoc;
    ShaderUniform mGBufferNormalSamplerLoc;
    ShaderUniform mGBufferAlbedoSamplerLoc;
};

typedef RefPointer<VPLIndirectLightingScreenQuad> VPLIndirectLightingScreenQuadPtr;

}

#endif