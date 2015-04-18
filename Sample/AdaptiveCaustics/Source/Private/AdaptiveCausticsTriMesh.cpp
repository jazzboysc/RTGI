#include "AdaptiveCausticsTriMesh.h"
#include "AdaptiveCausticsApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
AdaptiveCausticsTriMesh::AdaptiveCausticsTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
}
//----------------------------------------------------------------------------
AdaptiveCausticsTriMesh::~AdaptiveCausticsTriMesh()
{
}
//----------------------------------------------------------------------------
void AdaptiveCausticsTriMesh::OnGetShaderConstants()
{
	auto program = mMaterial->GetProgram(0, AdaptiveCausticsApp::SMP_GBuffer);
	program->GetUniformLocation(&mWorldLoc, "World");
	program->GetUniformLocation(&mViewLoc, "View");
	program->GetUniformLocation(&mProjLoc, "Proj");
	program->GetUniformLocation(&mMaterialColorLoc, "materialColor");
	program = mMaterial->GetProgram(0, AdaptiveCausticsApp::SMP_Resource);
	program->GetUniformLocation(&mWorldLoc2, "World");
	program->GetUniformLocation(&mViewLoc2, "View");
	program->GetUniformLocation(&mProjLoc2, "Proj");
	program = mMaterial->GetProgram(0, AdaptiveCausticsApp::SMP_ShadowMap);
	program->GetUniformLocation(&mWorldLoc3, "World");
	program->GetUniformLocation(&mViewLoc3, "View");
	program->GetUniformLocation(&mLightProjectorNearFarLoc, "LightProjectorNearFar");
	program->GetUniformLocation(&mTessLevelLoc, "TessLevel");
}
//----------------------------------------------------------------------------
void AdaptiveCausticsTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
	glm::mat4 worldTrans = mSpatialInfo->GetWorldTransform();

	switch (pass)
	{
	default:
		break;
	case AdaptiveCausticsApp::SMP_GBuffer:
		mWorldLoc.SetValue(worldTrans);
		mMaterialColorLoc.SetValue(MaterialColor);
		if (mCamera)
		{
			glm::mat4 viewTrans = mCamera->GetViewTransform();
			mViewLoc.SetValue(viewTrans);

			glm::mat4 projTrans = mCamera->GetProjectionTransform();
			mProjLoc.SetValue(projTrans);
		}
		break;
	case AdaptiveCausticsApp::SMP_Resource:
		mWorldLoc2.SetValue(worldTrans);
		if (mCamera)
		{
			glm::mat4 viewTrans = mCamera->GetViewTransform();
			mViewLoc2.SetValue(viewTrans);

			glm::mat4 projTrans = mCamera->GetProjectionTransform();
			mProjLoc2.SetValue(projTrans);
		}
		break;
	case AdaptiveCausticsApp::SMP_ShadowMap:
		mWorldLoc3.SetValue(worldTrans);
		mTessLevelLoc.SetValue(TessLevel);
		if (mCamera)
		{
			glm::mat4 viewTrans = mCamera->GetViewTransform();
			mViewLoc3.SetValue(viewTrans);

			float nearFarPlane[2];
			mCamera->GetNearFarPlane(nearFarPlane);
			mLightProjectorNearFarLoc.SetValue(nearFarPlane);
		}
		break;
	}
}
//----------------------------------------------------------------------------