#include "RayBundleTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RayBundleTriMesh::RayBundleTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	IsLight(false),
	MaterialColor(0.75f, 0.75f, 0.75f),
	EmissionColor(0.0f, 0.0f, 0.0f)
{
}
//----------------------------------------------------------------------------
RayBundleTriMesh::~RayBundleTriMesh()
{
}
//----------------------------------------------------------------------------
void RayBundleTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	// Get pass 1 uniform locations.
	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mIsLightLoc0, "isLight");

    // Get pass 2 uniform locations.
	mMaterial->GetProgram(0, 1);
    program->GetUniformLocation(&mWorldLoc2, "World");
    program->GetUniformLocation(&mViewLoc2, "View");
    program->GetUniformLocation(&mProjLoc2, "Proj");
    program->GetUniformLocation(&mIsLightLoc, "isLight");
    program->GetUniformLocation(&mMaterialColorLoc, "materialColor");
    program->GetUniformLocation(&mEmissionColorLoc, "emissionColor");
}
//----------------------------------------------------------------------------
void RayBundleTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
    // Update pass 1 uniform data.
    if( pass == 0 )
    {
		TriangleMesh::OnUpdateShaderConstants(technique, pass);
        mIsLightLoc0.SetValue(IsLight);
	}

    // Update pass 2 uniform data.
    if( pass == 1 )
    {
        mWorldLoc2.SetValue(mWorldTransform);
        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            mViewLoc2.SetValue(viewTrans);
            
            mat4 projTrans = mCamera->GetProjectionTransform();
            mProjLoc2.SetValue(projTrans);
        }

        mIsLightLoc.SetValue(IsLight);
        mMaterialColorLoc.SetValue(MaterialColor);
        mEmissionColorLoc.SetValue(EmissionColor);
	}
}
//----------------------------------------------------------------------------