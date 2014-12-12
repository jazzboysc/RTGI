#include "SimpleVoxelizationTriMesh.h"
#include "SimpleVoxelizationApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SimpleVoxelizationTriMesh::SimpleVoxelizationTriMesh(Material* material, 
    Camera* camera)
	:
	TriangleMesh(material, camera),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
    SceneBB = 0;
}
//----------------------------------------------------------------------------
SimpleVoxelizationTriMesh::~SimpleVoxelizationTriMesh()
{
}
//----------------------------------------------------------------------------
void SimpleVoxelizationTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	// Get pass 1 uniform locations.
    ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mSceneBBCenterLoc, "SceneBBCenter");
    program->GetUniformLocation(&mSceneBBExtensionLoc, "SceneBBExtension");
    program->GetUniformLocation(&mMaterialColorLoc, "MaterialColor");
    program->GetUniformLocation(&mDimLoc, "dim");
    program->GetUniformLocation(&mInv2SceneBBExtensionLoc, "Inv2SceneBBExtension");

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
void SimpleVoxelizationTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
    // Update pass 1 uniform data.
    if( pass == 0 )
    {
		TriangleMesh::OnUpdateShaderConstants(technique, pass);

        vec3 center = SceneBB->GetBoxCenter();
        vec3 extension = SceneBB->GetExtension();
        vec3 inv2extension = vec3(1.0f / (2.0f*extension.x), 1.0f / (2.0f*extension.y), 1.0f / (2.0f*extension.z));

        mSceneBBCenterLoc.SetValue(center);
        mSceneBBExtensionLoc.SetValue(extension);
        mMaterialColorLoc.SetValue(MaterialColor);
        mDimLoc.SetValue(SimpleVoxelizationApp::VOXEL_DIMENSION);
        mInv2SceneBBExtensionLoc.SetValue(inv2extension);
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
            mDimLoc2.SetValue(SimpleVoxelizationApp::VOXEL_DIMENSION);

            SimpleVoxelizationApp* app = (SimpleVoxelizationApp*)Application::GetInstance();
            if( app->mShowMode == SimpleVoxelizationApp::SM_WorldPosition )
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