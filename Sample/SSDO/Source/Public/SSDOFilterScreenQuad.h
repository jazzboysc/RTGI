#ifndef RTGI_SSDOFilterScreenQuad_H
#define RTGI_SSDOFilterScreenQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/24/2014
//----------------------------------------------------------------------------
class SSDOFilterScreenQuad : public ScreenQuad
{
public:
	SSDOFilterScreenQuad(Material* material);
	virtual ~SSDOFilterScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	Texture2DPtr SSDOTexture;
	Texture2DPtr PositionTexture;
	Texture2DPtr NormalTexture;
	Texture2DPtr ColorTexture;
	Texture2DPtr DirectLightingTexture;

	float PositionThreshold;
	float NormalThreshold;
	float MaxRadiance;
	int KernelSize;

private:
	ShaderUniform mSSDOSamplerLoc;
    ShaderUniform mPositionSamplerLoc;
    ShaderUniform mNormalSamplerLoc;
    ShaderUniform mColorSamplerLoc;
    ShaderUniform mDirectLightingSamplerLoc;
    ShaderUniform mPositionThresholdLoc;
    ShaderUniform mNormalThresholdLoc;
    ShaderUniform mMaxRadianceLoc;
    ShaderUniform mKernelSizeLoc;
};

typedef RefPointer<SSDOFilterScreenQuad> SSDOFilterScreenQuadPtr;

}

#endif