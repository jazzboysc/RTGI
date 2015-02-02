#include "CausticsScreenQuad.h"

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
	PositionTexture = 0;
	NormalTexture = 0;
	ReflectanceTexture = 0;
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



		PositionTexture->BindToSampler(0, &sampler);
		NormalTexture->BindToSampler(1, &sampler);
		ReflectanceTexture->BindToSampler(2, &sampler);
		ReracterPositionTexture->BindToSampler(3, &sampler);
		ReracterNormalTexture->BindToSampler(4, &sampler);
		ReceiverPositionTexture->BindToSampler(5, &sampler);

		CubeTexture->BindToSampler(6, &sampler);

		mPositionSamplerLoc.SetValue(0);
		mNormalSamplerLoc.SetValue(1);
		mReflectanceSamplerLoc.SetValue(2);
		mRefracterPositionSamplerLoc.SetValue(3);
		mRefracterNormalSamplerLoc.SetValue(4);
		mReceiverPositionSamplerLoc.SetValue(5);

		mCubeTextureLoc.SetValue(6);
	}

	if (pass == 1)
	{
		GPU_DEVICE_FUNC_SetUniformValueMat4(mWorldLoc2, mWorldTransform);
		if (mCamera)
		{
			glm::mat4 viewTrans = mCamera->GetViewTransform();
			GPU_DEVICE_FUNC_SetUniformValueMat4(mViewLoc2, viewTrans);

			glm::mat4 projTrans = mCamera->GetProjectionTransform();
			GPU_DEVICE_FUNC_SetUniformValueMat4(mProjLoc2, projTrans);
		}

		mLightViewLoc2.SetValue(Light->GetProjector()->GetViewTransform());
		mLightProjLoc2.SetValue(Light->GetProjector()->GetProjectionTransform());


		IntersectionPositionTexture->BindToSampler(0, &sampler);

		mIntersectionPositionSamplerLoc.SetValue(0);
		mCausticsMapResolutionLoc.SetValue(CausticsMapsResolution);
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

	program->GetUniformLocation(&mRefracterPositionSamplerLoc, "refracterPositionLightSampler");
	program->GetUniformLocation(&mRefracterNormalSamplerLoc, "refracterNormalLightSampler");
	program->GetUniformLocation(&mReceiverPositionSamplerLoc, "receiverPositionLightSampler");
	program->GetUniformLocation(&mCubeTextureLoc, "cubeSampler");

	program = mMaterial->GetProgram(0, 1);
	program->GetUniformLocation(&mWorldLoc2, "World");
	program->GetUniformLocation(&mViewLoc2, "View");
	program->GetUniformLocation(&mProjLoc2, "Proj");

	program->GetUniformLocation(&mIntersectionPositionSamplerLoc, "intersectionPositionSampler");
	program->GetUniformLocation(&mCausticsMapResolutionLoc, "causticsMapResolution");
	program->GetUniformLocation(&mLightViewLoc2, "lightView");
	program->GetUniformLocation(&mLightProjLoc2, "lightProj");
}
//----------------------------------------------------------------------------