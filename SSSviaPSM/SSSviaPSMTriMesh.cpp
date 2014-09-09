#include "SSSviaPSMTriMesh.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SSSviaPSMTriMesh::SSSviaPSMTriMesh(Material* material, Camera* camera)
	:
	TriangleMesh(material, camera),
	IsLight(false),
    IsSSS(false),
	MaterialColor(0.75f, 0.75f, 0.75f)
{
    mWorldLoc2 = -1; 
    mViewLoc2 = -1;
    mProjLoc2 = -1;
    mLightProjectorViewLoc = -1;
    mShadowMapSamplerLoc = -1;
    mLightPositionWorldLoc = -1;
    mLightColorLoc = -1;
    mMaterialColorLoc = -1;
    mLightProjectorNearFarLoc = -1;
    mLightProjectorNearFarLoc2 = -1;
    mIsSSSLoc = -1;

    LightProjector = 0;
    ShadowMap = 0;
}
//----------------------------------------------------------------------------
SSSviaPSMTriMesh::~SSSviaPSMTriMesh()
{
}
//----------------------------------------------------------------------------
void SSSviaPSMTriMesh::OnGetShaderConstants()
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
    mLightProjectorViewLoc = glGetUniformLocation(program, "LightProjectorView");
    mShadowMapSamplerLoc = glGetUniformLocation(program, "shadowMapSampler");
    mLightPositionWorldLoc = glGetUniformLocation(program, "LightPositionWorld");
    mCameraPositionWorldLoc = glGetUniformLocation(program, "CameraPositionWorld");
    mLightColorLoc = glGetUniformLocation(program, "LightColor");
    mMaterialColorLoc = glGetUniformLocation(program, "MaterialColor");
    mLightProjectorNearFarLoc2 =
        glGetUniformLocation(program, "LightProjectorNearFar");
    mIsSSSLoc = glGetUniformLocation(program, "IsSSS");
}
//----------------------------------------------------------------------------
void SSSviaPSMTriMesh::OnUpdateShaderConstants(int technique, int pass)
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
        glUniform3fv(mMaterialColorLoc, 1, (GLfloat*)&MaterialColor);
        glUniform1i(mIsSSSLoc, IsSSS);

        if( mCamera )
        {
            mat4 viewTrans = mCamera->GetViewTransform();
            glUniformMatrix4fv(mViewLoc2, 1, GL_TRUE, viewTrans);

            mat4 projTrans = mCamera->GetProjectionTransform();
            glUniformMatrix4fv(mProjLoc2, 1, GL_TRUE, projTrans);

            vec3 cameraPosition = mCamera->GetLocation();
            glUniform3fv(mCameraPositionWorldLoc, 1, (GLfloat*)&cameraPosition);
        }

        if( LightProjector )
        {
            mat4 viewTrans = LightProjector->GetViewTransform();
            glUniformMatrix4fv(mLightProjectorViewLoc, 1, GL_TRUE, viewTrans);

            vec3 lightPosition = LightProjector->GetLocation();
            glUniform3fv(mLightPositionWorldLoc, 1, (GLfloat*)&lightPosition);

            float nearFarPlane[2];
            LightProjector->GetNearFarPlane(nearFarPlane);
            glUniform2fv(mLightProjectorNearFarLoc2, 1, nearFarPlane);
        }

        if( ShadowMap )
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ShadowMap->GetTexture());
            glUniform1i(mShadowMapSamplerLoc, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        }
    }
}
//----------------------------------------------------------------------------