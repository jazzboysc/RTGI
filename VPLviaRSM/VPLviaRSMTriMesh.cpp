#include "VPLviaRSMTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLviaRSMTriMesh::VPLviaRSMTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
}
//----------------------------------------------------------------------------
VPLviaRSMTriMesh::~VPLviaRSMTriMesh()
{
    //ShadowMap = 0;
    //VPLShadowMap = 0;
}
//----------------------------------------------------------------------------
void VPLviaRSMTriMesh::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	// Get pass 1 uniform locations.
    GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
    mLightProjectorNearFarLoc = 
        glGetUniformLocation(program, "LightProjectorNearFar");

    // Get pass 2 uniform locations.
    program = mMaterial->GetProgram(0, 1)->GetProgram();
    mWorldLoc2 = glGetUniformLocation(program, "World");
    mViewLoc2 = glGetUniformLocation(program, "View");
    mProjLoc2 = glGetUniformLocation(program, "Proj");
    mMaterialColorLoc2 = glGetUniformLocation(program, "MaterialColor");

    // Get pass 3 uniform locations.
    program = mMaterial->GetProgram(0, 2)->GetProgram();
    mWorldLoc3 = glGetUniformLocation(program, "World");
    mViewLoc3 = glGetUniformLocation(program, "View");
    mProjLoc3 = glGetUniformLocation(program, "Proj");
    mMaterialColorLoc3 = glGetUniformLocation(program, "MaterialColor");

    //// Get pass 3 uniform locations.
    //program = mMaterial->GetProgram(0, 2)->GetProgram();
    //mWorldLoc3 = glGetUniformLocation(program, "World");
    //mViewLoc3 = glGetUniformLocation(program, "View");
    //mProjLoc3 = glGetUniformLocation(program, "Proj");
    //mLightProjectorViewLoc3 = glGetUniformLocation(program, "LightProjectorView");
    //mShadowMapSamplerLoc3 = glGetUniformLocation(program, "shadowMapSampler");
    //mLightPositionWorldLoc3 = glGetUniformLocation(program, "LightPositionWorld");
    //mLightColorLoc3 = glGetUniformLocation(program, "LightColor");
    //mMaterialColorLoc3 = glGetUniformLocation(program, "MaterialColor");
    //mLightProjectorNearFarLoc3 =
    //    glGetUniformLocation(program, "LightProjectorNearFar");

    //// Get pass 4 uniform locations.
    //program = mMaterial->GetProgram(0, 3)->GetProgram();
    //mWorldLoc4 = glGetUniformLocation(program, "World");
    //mViewLoc4 = glGetUniformLocation(program, "View");
    //mProjLoc4 = glGetUniformLocation(program, "Proj");
    //mLightProjectorViewLoc4 = glGetUniformLocation(program, "LightProjectorView");
    //mShadowMapSamplerLoc4 = glGetUniformLocation(program, "shadowMapSampler");
    //mLightPositionWorldLoc4 = glGetUniformLocation(program, "LightPositionWorld");
    //mLightColorLoc4 = glGetUniformLocation(program, "LightColor");
    //mMaterialColorLoc4 = glGetUniformLocation(program, "MaterialColor");
    //mLightProjectorNearFarLoc4 =
    //    glGetUniformLocation(program, "LightProjectorNearFar");
}
//----------------------------------------------------------------------------
void VPLviaRSMTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
    // Update pass 1 uniform data.
    if( pass == 0 )
    {
		TriangleMesh::OnUpdateShaderConstants(technique, pass);

        if( mCamera )
        {
            float nearFarPlane[2];
            mCamera->GetNearFarPlane(nearFarPlane);
            glUniform2fv(mLightProjectorNearFarLoc, 1, nearFarPlane);
        }
	}

    // Update pass 2 uniform data.
    if( pass == 1 )
    {
        glUniformMatrix4fv(mWorldLoc2, 1, GL_TRUE, mWorldTransform);
        glUniform3fv(mMaterialColorLoc2, 1, (GLfloat*)&MaterialColor);

        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            glUniformMatrix4fv(mViewLoc2, 1, GL_TRUE, viewTrans);

            mat4 projTrans = mCamera->GetProjectionTransform();
            glUniformMatrix4fv(mProjLoc2, 1, GL_TRUE, projTrans);
        }
    }

    // Update pass 3 uniform data.
    if( pass == 2 )
    {
        glUniformMatrix4fv(mWorldLoc3, 1, GL_TRUE, mWorldTransform);
        glUniform3fv(mMaterialColorLoc3, 1, (GLfloat*)&MaterialColor);

        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            glUniformMatrix4fv(mViewLoc3, 1, GL_TRUE, viewTrans);

            mat4 projTrans = mCamera->GetProjectionTransform();
            glUniformMatrix4fv(mProjLoc3, 1, GL_TRUE, projTrans);
        }
    }
}
//----------------------------------------------------------------------------