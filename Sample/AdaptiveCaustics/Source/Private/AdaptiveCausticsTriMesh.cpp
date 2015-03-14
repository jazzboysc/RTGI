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
	auto program = mMaterial->GetProgram(0, AdaptiveCausticsApp::SMP_Resource);
	program->GetUniformLocation(&mWorldLoc, "World");
	program->GetUniformLocation(&mViewLoc, "View");
	program->GetUniformLocation(&mProjLoc, "Proj");
}
//----------------------------------------------------------------------------
void AdaptiveCausticsTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
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