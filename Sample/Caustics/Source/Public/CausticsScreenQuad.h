#pragma once
#include "ScreenQuad.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/23/2014
//----------------------------------------------------------------------------
	class CausticsScreenQuad : public ScreenQuad
{
public:
	CausticsScreenQuad(Material* material, Camera* camera);
	virtual ~CausticsScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	// Pass 0
	LightPtr Light;
	float RefractionIndex;

	Texture2DPtr PositionTexture;
	Texture2DPtr NormalTexture;
	Texture2DPtr ReflectanceTexture;

	Texture2DPtr ReracterPositionTexture;
	Texture2DPtr ReracterNormalTexture;
	Texture2DPtr ReceiverPositionTexture;

	TextureCubePtr CubeTexture;

	// Pass 1
	Texture2DPtr IntersectionPositionTexture;
	int CausticsMapsResolution;


private:
	// Pass 0
	ShaderUniform mLightPositionLoc;
	ShaderUniform mLightViewLoc;
	ShaderUniform mLightProjLoc;
	ShaderUniform mLightColorLoc;
	
	ShaderUniform mRefractionIndexLoc;

    ShaderUniform mPositionSamplerLoc;
    ShaderUniform mNormalSamplerLoc;
	ShaderUniform mReflectanceSamplerLoc;
	ShaderUniform mRefracterPositionSamplerLoc;
	ShaderUniform mRefracterNormalSamplerLoc;
	ShaderUniform mReceiverPositionSamplerLoc;

	ShaderUniform mCubeTextureLoc;


	// Pass 1
	ShaderUniform mCausticsMapResolutionLoc;
	ShaderUniform mIntersectionPositionSamplerLoc;

	ShaderUniform mWorldLoc2, mViewLoc2, mProjLoc2;
	ShaderUniform mLightViewLoc2;
	ShaderUniform mLightProjLoc2;
};

	typedef RefPointer<CausticsScreenQuad> CausticsScreenQuadPtr;

}
