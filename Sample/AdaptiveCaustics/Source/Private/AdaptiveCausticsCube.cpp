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
}
//----------------------------------------------------------------------------
void AdaptiveCausticsCube::OnUpdateShaderConstants(int technique, int pass)
{
	TriangleMesh::OnUpdateShaderConstants(technique, pass);

	SamplerDesc sampler;
	sampler.MinFilter = FT_Linear_Linear;
	sampler.MagFilter = FT_Linear;
	sampler.WrapS = WT_Clamp;
	sampler.WrapT = WT_Clamp;
	sampler.WrapR = WT_Clamp;

	switch (technique)
	{
	default:
		break;
	case AdaptiveCausticsApp::SMP_Resource:
		glm::mat4 worldTrans = mSpatialInfo->GetWorldTransform();
		mWorldLoc.SetValue(worldTrans);
		if (mCamera)
		{
			glm::mat4 viewTrans = mCamera->GetViewTransform();
			mViewLoc.SetValue(viewTrans);

			glm::mat4 projTrans = mCamera->GetProjectionTransform();
			mProjLoc.SetValue(projTrans);
		}
		break;
	}
}
//----------------------------------------------------------------------------