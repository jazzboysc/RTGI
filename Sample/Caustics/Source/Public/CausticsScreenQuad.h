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

	Texture2DPtr RefracterPositionLightTexture;
	Texture2DPtr RefracterNormalLightTexture;
	Texture2DPtr ReceiverPositionLightTexture;

	TextureCubePtr CubeTexture;

	// Pass 1, 2 Gaussian Blur X and Y direction
	Texture2DPtr CausticsMapTexture;

	// Pass final render receiver
	Texture2DPtr BlurredCausticsMapTexture;

	Texture2DPtr ReceiverPositionTexture;
	Texture2DPtr ReceiverNormalTexture;

	// Pass final render Refractive obj
	Texture2DPtr RefracterPositionTexture;
	Texture2DPtr RefracterNormalTexture;
	Texture2DPtr ReceiverColorTexture;
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

	// Pass FInal Render
	ShaderUniform mReceiverPositionSamplerLoc;
	ShaderUniform mReceiverNormalSamplerLoc;
	ShaderUniform mReceiverColorSamplerLoc;
	ShaderUniform mBlurredCausticsMapSamplerLoc;
	ShaderUniform mWorldLoc3, mViewLoc3, mProjLoc3;
	ShaderUniform mLightPositionLoc3;
	ShaderUniform mLightViewLoc3;
	ShaderUniform mLightProjLoc3;
	ShaderUniform mLightColorLoc3;
};

	typedef RefPointer<CausticsScreenQuad> CausticsScreenQuadPtr;

}
