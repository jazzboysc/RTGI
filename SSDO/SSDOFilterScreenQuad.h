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
	virtual void OnUpdateShaderConstants();
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
	GLint mSSDOSamplerLoc;
	GLint mPositionSamplerLoc;
	GLint mNormalSamplerLoc;
	GLint mColorSamplerLoc;
	GLint mDirectLightingSamplerLoc;
	GLint mPositionThresholdLoc;
	GLint mNormalThresholdLoc;
	GLint mMaxRadianceLoc;
	GLint mKernelSizeLoc;
};

typedef RefPointer<SSDOFilterScreenQuad> SSDOFilterScreenQuadPtr;

}

#endif