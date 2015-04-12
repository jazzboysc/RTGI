#include "AdaptiveCausticsCube.h"
#include "AdaptiveCausticsApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
AdaptiveCausticsCube::AdaptiveCausticsCube(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
}
//----------------------------------------------------------------------------
AdaptiveCausticsCube::~AdaptiveCausticsCube()
{
}
//----------------------------------------------------------------------------
void AdaptiveCausticsCube::OnGetShaderConstants()
{
	auto program = mMaterial->GetProgram(0, AdaptiveCausticsApp::SMP_Resource);
	program->GetUniformLocation(&mWorldLoc, "World");
	program->GetUniformLocation(&mViewLoc, "View");
	program->GetUniformLocation(&mProjLoc, "Proj");
	program = mMaterial->GetProgram(0, AdaptiveCausticsApp::SMP_ShadowMap);
	program->GetUniformLocation(&mWorldLoc2, "World");
	program->GetUniformLocation(&mViewLoc2, "View");
	program->GetUniformLocation(&mLightProjectorNearFarLoc, "LightProjectorNearFar");
	program->GetUniformLocation(&mTessLevelLoc, "TessLevel");
}
//----------------------------------------------------------------------------
void AdaptiveCausticsCube::OnUpdateShaderConstants(int technique, int pass)
{
	SamplerDesc sampler;
	sampler.MinFilter = FT_Linear_Linear;
	sampler.MagFilter = FT_Linear;
	sampler.WrapS = WT_Clamp;
	sampler.WrapT = WT_Clamp;
	sampler.WrapR = WT_Clamp;

	glm::mat4 worldTrans = mSpatialInfo->GetWorldTransform();

	switch (pass)
	{
	default:
		break;
	case AdaptiveCausticsApp::SMP_Resource:
		mWorldLoc.SetValue(worldTrans);
		if (mCamera)
		{
			glm::mat4 viewTrans = mCamera->GetViewTransform();
			mViewLoc.SetValue(viewTrans);

			glm::mat4 projTrans = mCamera->GetProjectionTransform();
			mProjLoc.SetValue(projTrans);
		}
		break;
	case AdaptiveCausticsApp::SMP_ShadowMap:
		mWorldLoc2.SetValue(worldTrans);
		mTessLevelLoc.SetValue(TessLevel);
		if (mCamera)
		{
			glm::mat4 viewTrans = mCamera->GetViewTransform();
			mViewLoc2.SetValue(viewTrans);

			float nearFarPlane[2];
			mCamera->GetNearFarPlane(nearFarPlane);
			mLightProjectorNearFarLoc.SetValue(nearFarPlane);
		}
		break;
	}
}
//----------------------------------------------------------------------------