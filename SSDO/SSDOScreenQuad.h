#ifndef RTGI_SSDOScreenQuad_H
#define RTGI_SSDOScreenQuad_H

#include "ScreenQuad.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/23/2014
//----------------------------------------------------------------------------
class SSDOScreenQuad : public ScreenQuad
{
public:
	SSDOScreenQuad(Material* material, Camera* camera);
	virtual ~SSDOScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	float SampleRadius;
	float Strength;
	float Singularity;
	float DepthBias;
	float BounceStrength;
	float BounceSingularity;
	int SampleCount;
	int PatternSize;

	Texture2DPtr PositionTexture;
	Texture2DPtr NormalTexture;
	Texture2DPtr ColorTexture;
	Texture2DPtr DirectLightingTexture;
	Texture2DPtr RandomTexture;
	Texture2DPtr EnvMapTexture;

private:
	ShaderUniform mSampleRadiusLoc;
    ShaderUniform mStrengthLoc;
    ShaderUniform mSingularityLoc;
    ShaderUniform mDepthBiasLoc;
    ShaderUniform mBounceStrengthLoc;
    ShaderUniform mBounceSingularityLoc;
    ShaderUniform mSampleCountLoc;
    ShaderUniform mPatternSizeLoc;

    ShaderUniform mPositionSamplerLoc;
    ShaderUniform mNormalSamplerLoc;
    ShaderUniform mColorSamplerLoc;
    ShaderUniform mDirectLightingSamplerLoc;
    ShaderUniform mRandomSamplerLoc;
    ShaderUniform mEnvMapSamplerLoc;
};

typedef RefPointer<SSDOScreenQuad> SSDOScreenQuadPtr;

}

#endif