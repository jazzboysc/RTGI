#include "SVOTriMesh.h"
#include "SVOApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SVOTriMesh::SVOTriMesh(Material* material, 
    Camera* camera)
	:
	TriangleMesh(material, camera),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
    SceneBB = 0;
}
//----------------------------------------------------------------------------
SVOTriMesh::~SVOTriMesh()
{
}
//----------------------------------------------------------------------------
void SVOTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	// Get pass 1 uniform locations.
    ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mMaterialColorLoc, "MaterialColor");

    // Get pass 2 uniform locations.
    program = mMaterial->GetProgram(0, 1);
    program->GetUniformLocation(&mSceneBBCenterLoc2, "SceneBBCenter");
    program->GetUniformLocation(&mSceneBBExtensionLoc2, "SceneBBExtension");
    program->GetUniformLocation(&mWorldLoc2, "World");
    program->GetUniformLocation(&mViewLoc2, "View");
    program->GetUniformLocation(&mProjLoc2, "Proj");
    program->GetUniformLocation(&mDimLoc2, "dim");
    program->GetUniformLocation(&mShowWorldPositionLoc, "ShowWorldPosition");
}
//----------------------------------------------------------------------------
void SVOTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
    // Update pass 1 uniform data.
    if( pass == 0 )
    {
		TriangleMesh::OnUpdateShaderConstants(technique, pass);

        mMaterialColorLoc.SetValue(MaterialColor);
	}

    if( pass == 1 )
    {
        mWorldLoc2.SetValue(mWorldTransform);
        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            mViewLoc2.SetValue(viewTrans);

            mat4 projTrans = mCamera->GetProjectionTransform();
            mProjLoc2.SetValue(projTrans);

            vec3 center = SceneBB->GetBoxCenter();
            vec3 extension = SceneBB->GetExtension();
            mSceneBBCenterLoc2.SetValue(center);
            mSceneBBExtensionLoc2.SetValue(extension);
            mDimLoc2.SetValue(SVOApp::VOXEL_DIMENSION);

            SVOApp* app = (SVOApp*)Application::GetInstance();
            if( app->mShowMode == SVOApp::SM_WorldPosition )
            {
                mShowWorldPositionLoc.SetValue(1);
            }
            else
            {
                mShowWorldPositionLoc.SetValue(0);
            }
        }
    }
}
//----------------------------------------------------------------------------