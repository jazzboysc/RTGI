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

	Texture2DPtr ReceiverPositionLightTexture;
	Texture2DPtr ReceiverNormalLightTexture;
	Texture2DPtr ReceiverReflectanceLightTexture;

	Texture2DPtr RefracterPositionLightTexture;
	Texture2DPtr RefracterNormalLightTexture;
	Texture2DPtr RefracterReflectanceLightTexture;

	TextureCubePtr CubeTexture;

	// Pass 1, 2 Gaussian Blur X and Y direction
	Texture2DPtr CausticsMapTexture;

	// Pass final render receiver
	Texture2DPtr BlurredCausticsMapTexture;
	Texture2DPtr ReceiverPositionTexture;
	Texture2DPtr ReceiverNormalTexture;
	Texture2DPtr ReceiverColorTexture;
	Texture2DPtr ShadowmapTexture;

	// Pass final render Refractive obj
	Texture2DPtr RefracPositionTexture;
	Texture2DPtr RefracNormalTexture;
	Texture2DPtr RefracColorTexture;
	TextureCubePtr CubeTexture2;
	Texture2DPtr ShadowmapTexture2;
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
	ShaderUniform mRefracterPositionLightSamplerLoc;
	ShaderUniform mRefracterNormalLightSamplerLoc;
	ShaderUniform mReceiverPositionLightSamplerLoc;

	ShaderUniform mCubeTextureLoc;

	// Pass 1, 2 Gaussian Blur X and Y direction
	ShaderUniform mCausticsMapSamplerLoc;
	ShaderUniform mCausticsMapSamplerLoc2;

	// Pass final render receiver
	ShaderUniform mReceiverPositionSamplerLoc;
	ShaderUniform mReceiverNormalSamplerLoc;
	ShaderUniform mReceiverColorSamplerLoc;
	ShaderUniform mBlurredCausticsMapSamplerLoc;
	ShaderUniform mWorldLoc3, mViewLoc3, mProjLoc3;
	ShaderUniform mLightPositionLoc3;
	ShaderUniform mLightViewLoc3;
	ShaderUniform mLightProjLoc3;
	ShaderUniform mLightColorLoc3;
	ShaderUniform mShadowMapTextureLoc;

	// pass final render refractive obj
	ShaderUniform mRefracPositionSamplerLoc;
	ShaderUniform mRefracNormalSamplerLoc;
	ShaderUniform mRefracColorSamplerLoc;
	ShaderUniform mWorldLoc4, mViewLoc4, mProjLoc4;
	ShaderUniform mLightPositionLoc4;
	ShaderUniform mLightViewLoc4;
	ShaderUniform mLightProjLoc4;
	ShaderUniform mLightColorLoc4;

	ShaderUniform mRefractionIndexLoc2;
	ShaderUniform mShadowMapTextureLoc2;

	// optional
	ShaderUniform mCubeTextureLoc2;
	};

	typedef RefPointer<CausticsScreenQuad> CausticsScreenQuadPtr;

}
