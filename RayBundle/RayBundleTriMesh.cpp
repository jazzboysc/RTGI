#include "RayBundleTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RayBundleTriMesh::RayBundleTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	IsLight(false),
	MaterialColor(0.75f, 0.75f, 0.75f),
	EmissionColor(0.0f, 0.0f, 0.0f),
	mIsLightLoc0(0),
	mWorldLoc2(0),
	mViewLoc2(0),
	mProjLoc2(0),
	mIsLightLoc(0),
	mMaterialColorLoc(0),
	mEmissionColorLoc(0)
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
	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
	mIsLightLoc0 = glGetUniformLocation(program, "isLight");

    // Get pass 2 uniform locations.
	program = mMaterial->GetProgram(0, 1)->GetProgram();
	mWorldLoc2 = glGetUniformLocation(program, "World");
	mViewLoc2 = glGetUniformLocation(program, "View");
	mProjLoc2 = glGetUniformLocation(program, "Proj");
	mIsLightLoc = glGetUniformLocation(program, "isLight");
	mMaterialColorLoc = glGetUniformLocation(program, "materialColor");
	mEmissionColorLoc = glGetUniformLocation(program, "emissionColor");
}
//----------------------------------------------------------------------------
void RayBundleTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
    // Update pass 1 uniform data.
    if( pass == 0 )
    {
		TriangleMesh::OnUpdateShaderConstants(technique, pass);
		glUniform1i(mIsLightLoc0, IsLight);
	}

    // Update pass 2 uniform data.
    if( pass == 1 )
    {
        glUniformMatrix4fv(mWorldLoc2, 1, GL_TRUE, mWorldTransform);
        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            glUniformMatrix4fv(mViewLoc2, 1, GL_TRUE, viewTrans);
            
            mat4 projTrans = mCamera->GetProjectionTransform();
            glUniformMatrix4fv(mProjLoc2, 1, GL_TRUE, projTrans);
        }

		glUniform1i(mIsLightLoc, IsLight);
		glUniform3fv(mMaterialColorLoc, 1, (GLfloat*)&MaterialColor);
		glUniform3fv(mEmissionColorLoc, 1, (GLfloat*)&EmissionColor);
	}
}
//----------------------------------------------------------------------------