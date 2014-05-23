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
	virtual void OnUpdateShaderConstants();
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
	GLint mSampleRadiusLoc;
	GLint mStrengthLoc;
	GLint mSingularityLoc;
	GLint mDepthBiasLoc;
	GLint mBounceStrengthLoc;
	GLint mBounceSingularityLoc;
	GLint mSampleCountLoc;
	GLint mPatternSizeLoc;

	GLint mPositionSamplerLoc;
	GLint mNormalSamplerLoc;
	GLint mColorSamplerLoc;
	GLint mDirectLightingSamplerLoc;
	GLint mRandomSamplerLoc;
	GLint mEnvMapSamplerLoc;
};

typedef RefPointer<SSDOScreenQuad> SSDOScreenQuadPtr;

}

#endif