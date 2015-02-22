#include "AdaptiveCausticsScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
CausticsScreenQuad::CausticsScreenQuad(Material* material, Camera* camera)
	:
	ScreenQuad(material, camera)
{
}
//----------------------------------------------------------------------------
CausticsScreenQuad::~CausticsScreenQuad()
{
	ReceiverPositionLightTexture = 0;
	ReceiverNormalLightTexture = 0;
	ReceiverReflectanceLightTexture = 0;
}
//----------------------------------------------------------------------------
void CausticsScreenQuad::OnUpdateShaderConstants(int technique, int pass)
{
	SamplerDesc sampler;
	sampler.MinFilter = FT_Nearest;
	sampler.MagFilter = FT_Nearest;
	sampler.WrapS = WT_Clamp;
	sampler.WrapT = WT_Clamp;
	sampler.WrapR = WT_Clamp;

	if (pass == 0)
	{
		TriangleMesh::OnUpdateShaderConstants(technique, pass);

		vec3 lightLoc = Light->GetLocation();
		mLightPositionLoc.SetValue(lightLoc);
		mLightViewLoc.SetValue(Light->GetProjector()->GetViewTransform());
		mLightProjLoc.SetValue(Light->GetProjector()->GetProjectionTransform());
		mLightColorLoc.SetValue(Light->Color);

		mRefractionIndexLoc.SetValue(RefractionIndex);



		ReceiverPositionLightTexture->BindToSampler(0, &sampler);
		ReceiverNormalLightTexture->BindToSampler(1, &sampler);
		ReceiverReflectanceLightTexture->BindToSampler(2, &sampler);
		RefracterPositionLightTexture->BindToSampler(3, &sampler);
		RefracterNormalLightTexture->BindToSampler(4, &sampler);
		ReceiverPositionLightTexture->BindToSampler(5, &sampler);

		CubeTexture->BindToSampler(6, &sampler);

		mPositionSamplerLoc.SetValue(0);
		mNormalSamplerLoc.SetValue(1);
		mReflectanceSamplerLoc.SetValue(2);
		mRefracterPositionLightSamplerLoc.SetValue(3);
		mRefracterNormalLightSamplerLoc.SetValue(4);
		mReceiverPositionLightSamplerLoc.SetValue(5);

		mCubeTextureLoc.SetValue(6);
	}

	if (pass == 1)
	{
		CausticsMapTexture->BindToSampler(0, &sampler);
		this->mCausticsMapSamplerLoc.SetValue(0);
	}

	if (pass == 2)
	{
		BlurredCausticsMapTexture->BindToSampler(0, &sampler);
		this->mCausticsMapSamplerLoc2.SetValue(0);
	}

	if (pass == 3)
	{
		GPU_DEVICE_FUNC_SetUniformValueMat4(mWorldLoc3, mWorldTransform);
		if (mCamera)
		{
			glm::mat4 viewTrans = mCamera->GetViewTransform();
			GPU_DEVICE_FUNC_SetUniformValueMat4(mViewLoc3, viewTrans);

			glm::mat4 projTrans = mCamera->GetProjectionTransform();
			GPU_DEVICE_FUNC_SetUniformValueMat4(mProjLoc3, projTrans);
		}

		mLightPositionLoc3.SetValue(Light->GetLocation());
		mLightViewLoc3.SetValue(Light->GetProjector()->GetViewTransform());
		mLightProjLoc3.SetValue(Light->GetProjector()->GetProjectionTransform());
		mLightColorLoc3.SetValue(Light->Color);

		BlurredCausticsMapTexture2->BindToSampler(0, &sampler);
		mBlurredCausticsMapSamplerLoc.SetValue(0);

		ReceiverPositionTexture->BindToSampler(1, &sampler);
		mReceiverPositionSamplerLoc.SetValue(1);

		ReceiverNormalTexture->BindToSampler(2, &sampler);
		mReceiverNormalSamplerLoc.SetValue(2);

		ReceiverColorTexture->BindToSampler(3, &sampler);
		mReceiverColorSamplerLoc.SetValue(3);

		ShadowmapTexture->BindToSampler(4, &sampler);
		mShadowMapTextureLoc.SetValue(4);
	}

	if (pass == 4)
	{
		GPU_DEVICE_FUNC_SetUniformValueMat4(mWorldLoc4, mWorldTransform);
		if (mCamera)
		{
			glm::mat4 viewTrans = mCamera->GetViewTransform();
			GPU_DEVICE_FUNC_SetUniformValueMat4(mViewLoc4, viewTrans);

			glm::mat4 projTrans = mCamera->GetProjectionTransform();
			GPU_DEVICE_FUNC_SetUniformValueMat4(mProjLoc4, projTrans);
		}

		mRefractionIndexLoc2.SetValue(RefractionIndex);

		mLightPositionLoc4.SetValue(Light->GetLocation());
		mLightViewLoc4.SetValue(Light->GetProjector()->GetViewTransform());
		mLightProjLoc4.SetValue(Light->GetProjector()->GetProjectionTransform());
		mLightColorLoc4.SetValue(Light->Color);

		RefracPositionTexture->BindToSampler(0, &sampler);
		mRefracPositionSamplerLoc.SetValue(0);

		RefracNormalTexture->BindToSampler(1, &sampler);
		mRefracNormalSamplerLoc.SetValue(1);

		RefracColorTexture->BindToSampler(2, &sampler);
		mRefracColorSamplerLoc.SetValue(2);

		CubeTexture->BindToSampler(3, &sampler);
		mCubeTextureLoc2.SetValue(3);

		ShadowmapTexture2->BindToSampler(4, &sampler);
		mShadowMapTextureLoc2.SetValue(4);
	}
}
//----------------------------------------------------------------------------
void CausticsScreenQuad::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	ShaderProgram* program = mMaterial->GetProgram(0, 0);

	program->GetUniformLocation(&mLightPositionLoc, "lightPosition");
	program->GetUniformLocation(&mLightViewLoc, "lightView");
	program->GetUniformLocation(&mLightProjLoc, "lightProj");
	program->GetUniformLocation(&mLightColorLoc, "lightColor");

	program->GetUniformLocation(&mRefractionIndexLoc, "refractionIndex");

    program->GetUniformLocation(&mPositionSamplerLoc, "positionSampler");
    program->GetUniformLocation(&mNormalSamplerLoc, "normalSampler");
	program->GetUniformLocation(&mReflectanceSamplerLoc, "reflectanceSampler");

	program->GetUniformLocation(&mRefracterPositionLightSamplerLoc, "refracterPositionLightSampler");
	program->GetUniformLocation(&mRefracterNormalLightSamplerLoc, "refracterNormalLightSampler");
	program->GetUniformLocation(&mReceiverPositionLightSamplerLoc, "receiverPositionLightSampler");
	program->GetUniformLocation(&mCubeTextureLoc, "cubeSampler");

	// Gaussian Blur
	program = mMaterial->GetProgram(0, 1);
	program->GetUniformLocation(&mCausticsMapSamplerLoc, "causticsMapSampler");
	program = mMaterial->GetProgram(0, 2);
	program->GetUniformLocation(&mCausticsMapSamplerLoc2, "causticsMapSampler");

	program = mMaterial->GetProgram(0, 3);
	program->GetUniformLocation(&mWorldLoc3, "World");
	program->GetUniformLocation(&mViewLoc3, "View");
	program->GetUniformLocation(&mProjLoc3, "Proj");

	program->GetUniformLocation(&mBlurredCausticsMapSamplerLoc, "blurredCausticsMapSampler");
	program->GetUniformLocation(&mReceiverPositionSamplerLoc, "receiverPositionSampler");
	program->GetUniformLocation(&mReceiverNormalSamplerLoc, "receiverNormalSampler");
	program->GetUniformLocation(&mReceiverColorSamplerLoc, "receiverReflectanceSampler");
	program->GetUniformLocation(&mShadowMapTextureLoc, "shadowmapSampler");

	program->GetUniformLocation(&mLightPositionLoc3, "lightPosition");
	program->GetUniformLocation(&mLightViewLoc3, "lightView");
	program->GetUniformLocation(&mLightProjLoc3, "lightProj");
	program->GetUniformLocation(&mLightColorLoc3, "lightColor");

	program = mMaterial->GetProgram(0, 4);
	program->GetUniformLocation(&mWorldLoc4, "World");
	program->GetUniformLocation(&mViewLoc4, "View");
	program->GetUniformLocation(&mProjLoc4, "Proj");

	program->GetUniformLocation(&mRefracPositionSamplerLoc, "refracPositionSampler");
	program->GetUniformLocation(&mRefracNormalSamplerLoc, "refracNormalSampler");
	program->GetUniformLocation(&mRefracColorSamplerLoc, "refracReflectanceSampler");
	program->GetUniformLocation(&mShadowMapTextureLoc2, "shadowmapSampler");
	
	program->GetUniformLocation(&mRefractionIndexLoc2, "refractionIndex");

	program->GetUniformLocation(&mLightPositionLoc4, "lightPosition");
	program->GetUniformLocation(&mLightViewLoc4, "lightView");
	program->GetUniformLocation(&mLightProjLoc4, "lightProj");
	program->GetUniformLocation(&mLightColorLoc4, "lightColor");
	program->GetUniformLocation(&mCubeTextureLoc2, "cubeSampler");

}
//----------------------------------------------------------------------------